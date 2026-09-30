#pragma once

#include <chrono>
#include <cstdint>
#include <string>

struct StockPrice
{
	std::string symbol;
	std::chrono::sys_time<std::chrono::milliseconds> timestamp;
	double open;
	double high;
	double low;
	double close;
	std::int64_t volume;
};
