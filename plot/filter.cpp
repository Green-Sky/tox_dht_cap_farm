#include "./csv2.hpp"

int main(int argc, char** argv) {
	csv2::Reader<
		csv2::delimiter<','>,
		csv2::quote_character<'"'>,
		csv2::first_row_is_header<false>,
		csv2::trim_policy::trim_whitespace
	> csv;

	if (!csv.mmap("plot.csv")) {
		std::cerr << "failed opening plot.csv\n";
		return 1;
	}

	size_t rows_discarded = 0;

	for (const auto row : csv) {
		bool discard_row = false;
		size_t cell_i = 0;
		for (const auto cell : row) {
			// 0 ts
			// 1 version
			// 2 nodes
			if (cell_i == 2) {
				std::string value;
				cell.read_value(value);

				// discard if < 10
				// hacky
				if (value.length() < 2) {
					discard_row = true;
				}
			}
			// 3 nodes with cap
			// 4 ratio

			cell_i++;
		}

		if (discard_row) {
			rows_discarded++;
		} else {
			std::string raw_row;
			row.read_raw_value(raw_row);
			std::cout << raw_row << "\n";
		}
	}

	std::cerr << "discarded " << rows_discarded << " rows\n";

	return 0;
}

