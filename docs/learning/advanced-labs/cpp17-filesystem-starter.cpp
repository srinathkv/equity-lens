#include "../exercises/LabChecks.h"

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

/** @brief Writes fixed lines then reads them back; return parsed line count. */
std::size_t roundTripLines(const std::filesystem::path& path) {
	(void)path;
	return 0;
}

int main() {
	LabChecks checks;
	const auto path = std::filesystem::temp_directory_path() / "equitylens-modern-cpp-lines.txt";
	checks.expectEqual("round-trip line count", static_cast<std::size_t>(3), roundTripLines(path));
	std::error_code error;
	std::filesystem::remove(path, error);
	return checks.finish("C++17 Filesystem and Streams");
}
