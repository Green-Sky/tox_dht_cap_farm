#include <fstream>
#include <chrono>
#include <ios>
#include <iostream>
#include <thread>
#include <filesystem>
#include <ctime>

#include <tox/tox.h>
#include <tox/tox_private.h>

#include <sodium.h>

// https://youtu.be/2DAetvbSzGM

// needs a custom toxcore build, since the api is private

// starts and bootstraps toxcore (new)
// idles for 5min
// grabs the dht numbers and appends them to csv

int main(int argc, char** argv) {
	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << " <path-to-csvs>\n";
		return 1;
	}

	if (std::filesystem::exists(argv[1])) {
		if (!std::filesystem::is_directory(argv[1])) {
			return 1;
		}
	} else {
		std::filesystem::create_directories(argv[1]);
	}

	if (!std::filesystem::is_directory(argv[1])) {
		return 1;
	}

	auto* options = tox_options_new(nullptr);
	if (options == nullptr) {
		return 1;
	}

	tox_options_default(options);
	auto* tox = tox_new(options, nullptr);

	std::cout << "Tox instance created\n";

	{ // bootstrap
		struct DHT_node {
			const char *ip;
			uint16_t port;
			const char key_hex[TOX_PUBLIC_KEY_SIZE*2 + 1]; // 1 for null terminator
			unsigned char key_bin[TOX_PUBLIC_KEY_SIZE];
		};

		DHT_node nodes[] {
			// TODO: more/diff nodes

			//{"tox.plastiras.org",	33445,	"8E8B63299B3D520FB377FE5100E65E3322F7AE5B20A0ACED2981769FC5B43725", {}}, // LU tha14
			{"104.244.74.69",		443,	"8E8B63299B3D520FB377FE5100E65E3322F7AE5B20A0ACED2981769FC5B43725", {}}, // LU tha14
			{"104.244.74.69",		33445,	"8E8B63299B3D520FB377FE5100E65E3322F7AE5B20A0ACED2981769FC5B43725", {}}, // LU tha14

			//{"tox2.plastiras.org",	33445,	"B6626D386BE7E3ACA107B46F48A5C4D522D29281750D44A0CBA6A2721E79C951", {}}, // DE tha14

			//{"tox4.plastiras.org",	33445,	"836D1DA2BE12FE0E669334E437BE3FB02806F1528C2B2782113E0910C7711409", {}}, // MD tha14
			{"37.221.66.161",	443,	"836D1DA2BE12FE0E669334E437BE3FB02806F1528C2B2782113E0910C7711409", {}}, // MD tha14
			{"37.221.66.161",	33445,	"836D1DA2BE12FE0E669334E437BE3FB02806F1528C2B2782113E0910C7711409", {}}, // MD tha14
		};

		for (size_t i = 0; i < sizeof(nodes)/sizeof(DHT_node); i++) {
			sodium_hex2bin(
				nodes[i].key_bin, sizeof(nodes[i].key_bin),
				nodes[i].key_hex, sizeof(nodes[i].key_hex)-1,
				NULL, NULL, NULL
			);
			tox_bootstrap(tox, nodes[i].ip, nodes[i].port, nodes[i].key_bin, NULL);
			// TODO: use extra tcp option to avoid error msgs
			tox_add_tcp_relay(tox, nodes[i].ip, nodes[i].port, nodes[i].key_bin, NULL);
		}
	}

	std::cout << "bootstrapped\n";

	using clock = std::chrono::steady_clock;
	const auto start_time = clock::now();
	const auto idle_duration = std::chrono::minutes{5};

	do {
		tox_iterate(tox, nullptr);
		std::this_thread::sleep_for(std::chrono::milliseconds{tox_iteration_interval(tox)});
	} while (clock::now() - start_time < idle_duration);

	std::cout << idle_duration.count() << "min passed\n";

	const auto num = tox_dht_get_num_closelist(tox);
	const auto num_cap = tox_dht_get_num_closelist_announce_capable(tox);
	std::cout << "dht num: " << num << "\n";
	std::cout << "dht cap num: " << num_cap << "\n";
	std::cout << "ratio: " << float(num_cap)/num << "\n";

	tox_kill(tox);

	// open appending date csv in data dir (potentially create)
	std::filesystem::path data_file_path {argv[1]};
	{ // date to filename
		std::time_t newt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		char time_string[std::size("yyyy-mm-dd")];
		std::strftime(std::data(time_string), std::size(time_string), "%F", std::gmtime(&newt));
		data_file_path /= time_string + std::string(".csv");
	}
	std::ofstream data_file(data_file_path, std::ios_base::out | std::ios_base::binary | std::ios_base::app);
	if (!data_file.is_open()) {
		std::cerr << "failed opening " << data_file_path.generic_u8string() << "\n";
		return 1;
	}

	// append line
	data_file
		<< std::chrono::duration_cast<std::chrono::seconds>(clock::now().time_since_epoch()).count() << ","
		<< TOXCORE_COMMIT_HASH << ","
		<< num << ","
		<< num_cap << ","
		<< float(num_cap)/num  << "\n"
	;

	return 0;
}

