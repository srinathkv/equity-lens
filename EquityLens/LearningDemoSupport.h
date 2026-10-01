#pragma once

#include <array>
#include <iomanip>
#include <ios>
#include <ostream>
#include <string>
#include <string_view>
#include <type_traits>

namespace EquityLensLearning
{
	/** @brief Writes an ASCII chapter banner and its concise topic summary.
	 * @param output Destination stream.
	 * @param standard Standard label shown in the banner.
	 * @param topics Short list of concepts demonstrated in the chapter.
	 */
	inline void writeChapterHeading(std::ostream& output, std::string_view standard, std::string_view topics)
	{
		output << "\n== " << standard << " ============================================================\n"
			<< "  " << topics << '\n';
	}

	/** @brief Writes a left-aligned label and a non-floating-point value.
	 * @tparam Value Streamable value type.
	 * @param output Destination stream.
	 * @param label Metric label, padded to the common display width.
	 * @param value Metric value.
	 */
	template<typename Value>
	inline void writeMetric(std::ostream& output, std::string_view label, const Value& value)
	{
		output << "  " << std::left << std::setw(34) << label << " : " << value << '\n' << std::right;
	}

	/** @brief Writes a floating-point metric to two decimals, restoring stream state.
	 * @param output Destination stream.
	 * @param label Metric label.
	 * @param value Metric value.
	 */
	inline void writeMetric(std::ostream& output, std::string_view label, double value)
	{
		const auto flags = output.flags();
		const auto precision = output.precision();
		output << "  " << std::left << std::setw(34) << label << " : "
			<< std::fixed << std::setprecision(2) << value << '\n';
		output.flags(flags);
		output.precision(precision);
	}

	/** @brief Writes a concise informational or feature-support note.
	 * @param output Destination stream.
	 * @param note Note text.
	 */
	inline void writeNote(std::ostream& output, std::string_view note)
	{
		output << "  Note: " << note << '\n';
	}

	/** @brief Small deterministic OHLCV sample used only by offline lessons. */
	struct PriceSample
	{
		std::string symbol;
		double open;
		double close;
		long long volume;
	};

	/** @brief Returns the immutable sample series shared by all lessons.
	 * @return Reference to process-lifetime static sample data.
	 */
	inline const std::array<PriceSample, 3>& samplePrices()
	{
		static const std::array<PriceSample, 3> prices{{
			{ "AAPL", 190.0, 193.5, 1200000 },
			{ "MSFT", 410.0, 407.0, 900000 },
			{ "NVDA", 880.0, 901.0, 2100000 }
		}};
		return prices;
	}
}
