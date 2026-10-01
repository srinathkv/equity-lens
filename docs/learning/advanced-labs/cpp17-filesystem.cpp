#include "../exercises/LabChecks.h"

#include <filesystem>
#include <fstream>
#include <string>

/** @brief Writes fixed lines then reads them back; return parsed line count. */
std::size_t roundTripLines(const std::filesystem::path& path) {
	{
		std::ofstream output(path, std::ios::binary | std::ios::trunc);
		if (!output) {
			return 0;
		}
		output << "AAPL,193.50\nMSFT,407.00\nNVDA,901.00\n";
		if (!output) {
			return 0;
		}
	}

	std::ifstream input(path, std::ios::binary);
	if (!input) {
		return 0;
	}
	std::size_t count = 0;
	std::string line;
	while (std::getline(input, line)) {
		if (!line.empty()) {
			++count;
		}
	}
	return input.eof() ? count : 0;
}

int main() {
	LabChecks checks;
	const auto path = std::filesystem::temp_directory_path() / "equitylens-modern-cpp-lines.txt";
	checks.expectEqual("round-trip line count", static_cast<std::size_t>(3), roundTripLines(path));
	std::error_code error;
	std::filesystem::remove(path, error);
	checks.expectEqual("temporary file removed", true, !std::filesystem::exists(path));
	return checks.finish("C++17 Filesystem and Streams");
}
