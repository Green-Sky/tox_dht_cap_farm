#include "./csv2.hpp"

#include <cmath>

int main(int argc, char** argv) {
	csv2::Reader<
		csv2::delimiter<','>,
		csv2::quote_character<'"'>,
		csv2::first_row_is_header<false>,
		csv2::trim_policy::trim_whitespace
	> csv;

	if (!csv.mmap("filtered.csv")) {
		std::cerr << "failed opening filtered.csv\n";
		return 1;
	}

	float avg_nodes = 0.f;
	float avg_nodes_with_cap = 0.f;

	std::string ts;
	std::string version;

	for (const auto row : csv) {
		size_t cell_i = 0;

		for (const auto cell : row) {
			// 0 ts
			if (cell_i == 0) {
				cell.read_value(ts);
			}

			// 1 version
			if (cell_i == 1) {
				cell.read_value(version);
			}

			// 2 nodes
			if (cell_i == 2) {
				std::string value;
				cell.read_value(value);
				const auto nodes = std::atoi(value.c_str());

				if (avg_nodes <= 0.00001f) {
					avg_nodes = nodes;
				} else {
					avg_nodes = std::lerp(avg_nodes, nodes, 0.1f);
				}
			}

			// 3 nodes with cap
			if (cell_i == 3) {
				std::string value;
				cell.read_value(value);
				const auto nodes_cap = std::atoi(value.c_str());

				if (avg_nodes_with_cap <= 0.00001f) {
					avg_nodes_with_cap = nodes_cap;
				} else {
					avg_nodes_with_cap = std::lerp(avg_nodes_with_cap, nodes_cap, 0.1f);
				}
			}

			// 4 ratio

			cell_i++;
		}

		std::cout << ts << "," << version << "," << avg_nodes << "," << avg_nodes_with_cap << "," << avg_nodes_with_cap/avg_nodes << "\n";

		ts.clear();
		version.clear();
	}

	return 0;
}

