#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Winhttp.h>

#include "AlphaVantageClient.h"
#include "AlphaVantageParsing.h"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <chrono>
#include <charconv>
#include <cmath>
#include <cstddef>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <system_error>
#include <thread>
#include <utility>

namespace
{
	using Json = nlohmann::json;
	using WinHttpHandle = std::unique_ptr<void, decltype(&WinHttpCloseHandle)>;

	class RetryableHttpStatus final : public std::runtime_error
	{
	public:
		using std::runtime_error::runtime_error;
	};

	constexpr unsigned maximumRequestAttempts = 3;

	bool isRetryableWinHttpError(int errorCode)
	{
		switch (errorCode)
		{
		case ERROR_WINHTTP_TIMEOUT:
		case ERROR_WINHTTP_NAME_NOT_RESOLVED:
		case ERROR_WINHTTP_CANNOT_CONNECT:
		case ERROR_WINHTTP_CONNECTION_ERROR:
		case ERROR_WINHTTP_RESEND_REQUEST:
			return true;
		default:
			return false;
		}
	}

	std::chrono::milliseconds retryDelay(unsigned failedAttempt)
	{
		return std::chrono::milliseconds{ 500 } * (1u << (failedAttempt - 1));
	}

	constexpr bool isAsciiAlphaNumeric(unsigned char character)
	{
		return (character >= 'A' && character <= 'Z') ||
			(character >= 'a' && character <= 'z') ||
			(character >= '0' && character <= '9');
	}

	[[noreturn]] void throwWinHttpError(const char* operation)
	{
		throw std::system_error(static_cast<int>(GetLastError()), std::system_category(), operation);
	}

	void sendRateLimitedRequest(HINTERNET request)
	{
		static std::mutex requestMutex;
		static std::chrono::steady_clock::time_point nextRequestAllowed{};
		constexpr auto minimumRequestInterval = std::chrono::milliseconds{ 1100 };

		std::lock_guard lock(requestMutex);
		if (const auto now = std::chrono::steady_clock::now(); now < nextRequestAllowed)
		{
			std::this_thread::sleep_until(nextRequestAllowed);
		}

		const BOOL sent = WinHttpSendRequest(request, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
			WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
		nextRequestAllowed = std::chrono::steady_clock::now() + minimumRequestInterval;
		if (!sent)
		{
			throwWinHttpError("Alpha Vantage request failed");
		}
	}

	std::string normalizeSymbol(std::string_view symbol)
	{
		if (symbol.empty())
		{
			throw std::invalid_argument("A stock symbol is required");
		}

		std::string normalized;
		normalized.reserve(symbol.size());
		for (const unsigned char character : symbol)
		{
			if (isAsciiAlphaNumeric(character))
			{
				normalized.push_back(static_cast<char>(character >= 'a' && character <= 'z'
					? character - ('a' - 'A') : character));
			}
			else if (character == '.' || character == '-' || character == '^')
			{
				normalized.push_back(static_cast<char>(character));
			}
			else
			{
				throw std::invalid_argument("Stock symbols may contain only letters, digits, '.', '-' or '^'");
			}
		}
		return normalized;
	}

	std::string urlEncode(std::string_view value)
	{
		constexpr char hex[] = "0123456789ABCDEF";
		std::string encoded;
		for (const unsigned char character : value)
		{
			if (isAsciiAlphaNumeric(character) || character == '-' || character == '_' || character == '.' || character == '~')
			{
				encoded.push_back(static_cast<char>(character));
			}
			else
			{
				encoded.push_back('%');
				encoded.push_back(hex[character >> 4]);
				encoded.push_back(hex[character & 0x0f]);
			}
		}
		return encoded;
	}

	std::wstring widenAscii(std::string_view value)
	{
		return std::wstring(value.begin(), value.end());
	}

	std::string getQuoteField(const Json& quote, const char* field)
	{
		const auto& value = quote.at(field);
		if (value.is_string())
		{
			return value.get<std::string>();
		}
		if (value.is_number())
		{
			return value.dump();
		}
		throw std::runtime_error(std::string("Unexpected Alpha Vantage field: ") + field);
	}

	double parseDouble(const Json& quote, const char* field)
	{
		const std::string text = getQuoteField(quote, field);
		double value = 0;
		const auto [end, error] = std::from_chars(
			text.data(), text.data() + text.size(), value, std::chars_format::general);
		if (error != std::errc{} || end != text.data() + text.size() || !std::isfinite(value))
		{
			throw std::runtime_error(std::string("Invalid Alpha Vantage numeric field: ") + field);
		}
		return value;
	}

	std::chrono::sys_time<std::chrono::milliseconds> parseTradingDate(std::string_view day);

	std::chrono::sys_time<std::chrono::milliseconds> parseTradingDay(const Json& quote)
	{
		const std::string day = getQuoteField(quote, "07. latest trading day");
		return parseTradingDate(day);
	}

	std::chrono::sys_time<std::chrono::milliseconds> parseTradingDate(std::string_view day)
	{
		if (day.size() != 10 || day[4] != '-' || day[7] != '-')
		{
			throw std::runtime_error("Invalid Alpha Vantage trading date");
		}

		for (std::size_t index = 0; index < day.size(); ++index)
		{
			if (index != 4 && index != 7 && (day[index] < '0' || day[index] > '9'))
			{
				throw std::runtime_error("Invalid Alpha Vantage trading date");
			}
		}

		const int year = (day[0] - '0') * 1000 + (day[1] - '0') * 100 + (day[2] - '0') * 10 + (day[3] - '0');
		const unsigned month = static_cast<unsigned>((day[5] - '0') * 10 + (day[6] - '0'));
		const unsigned dayOfMonth = static_cast<unsigned>((day[8] - '0') * 10 + (day[9] - '0'));
		const std::chrono::year_month_day tradingDate{
			std::chrono::year{ year }, std::chrono::month{ month }, std::chrono::day{ dayOfMonth }
		};
		if (!tradingDate.ok())
		{
			throw std::runtime_error("Invalid Alpha Vantage trading date");
		}
		return std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::sys_days{ tradingDate });
	}

	std::string readResponse(HINTERNET request)
	{
		constexpr std::size_t maximumResponseSize = 1024 * 1024;
		std::string response;
		for (;;)
		{
			DWORD available = 0;
			if (!WinHttpQueryDataAvailable(request, &available))
			{
				throwWinHttpError("Unable to read Alpha Vantage response");
			}
			if (available == 0)
			{
				return response;
			}
			if (response.size() + available > maximumResponseSize)
			{
				throw std::runtime_error("Alpha Vantage response exceeded the allowed size");
			}

			const std::size_t offset = response.size();
			response.resize(offset + available);
			DWORD downloaded = 0;
			if (!WinHttpReadData(request, response.data() + offset, available, &downloaded))
			{
				throwWinHttpError("Unable to read Alpha Vantage response");
			}
			response.resize(offset + downloaded);
			if (downloaded == 0)
			{
				return response;
			}
		}
	}

	Json fetchApiResponseOnce(std::string_view apiKey, std::string_view query)
	{
		const std::string path = "/query?" + std::string(query) + "&apikey=" + urlEncode(apiKey);
		const std::wstring widePath = widenAscii(path);

		WinHttpHandle session{ WinHttpOpen(L"EquityLens/1.0", WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,
			WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0), WinHttpCloseHandle };
		if (!session)
		{
			throwWinHttpError("Unable to initialize HTTPS networking");
		}
		if (!WinHttpSetTimeouts(session.get(), 5000, 5000, 10000, 10000))
		{
			throwWinHttpError("Unable to configure HTTPS request timeouts");
		}

		WinHttpHandle connection{ WinHttpConnect(session.get(), L"www.alphavantage.co", INTERNET_DEFAULT_HTTPS_PORT, 0), WinHttpCloseHandle };
		if (!connection)
		{
			throwWinHttpError("Unable to connect to Alpha Vantage");
		}

		WinHttpHandle request{ WinHttpOpenRequest(connection.get(), L"GET", widePath.c_str(), nullptr,
			WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE), WinHttpCloseHandle };
		if (!request)
		{
			throwWinHttpError("Unable to create Alpha Vantage request");
		}
		sendRateLimitedRequest(request.get());
		if (!WinHttpReceiveResponse(request.get(), nullptr))
		{
			throwWinHttpError("Alpha Vantage request failed");
		}

		DWORD statusCode = 0;
		DWORD statusCodeSize = sizeof(statusCode);
		if (!WinHttpQueryHeaders(request.get(), WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
			WINHTTP_HEADER_NAME_BY_INDEX, &statusCode, &statusCodeSize, WINHTTP_NO_HEADER_INDEX))
		{
			throwWinHttpError("Unable to read Alpha Vantage HTTP status");
		}
		if (statusCode == 408 || statusCode == 500 || statusCode == 502 || statusCode == 503 || statusCode == 504)
		{
			throw RetryableHttpStatus("Alpha Vantage returned temporary HTTP status " + std::to_string(statusCode));
		}
		if (statusCode == 429)
		{
			throw std::runtime_error("Alpha Vantage rate limit reached (HTTP status 429); retry the request later");
		}
		if (statusCode != 200)
		{
			throw std::runtime_error("Alpha Vantage returned HTTP status " + std::to_string(statusCode));
		}

		Json response = Json::parse(readResponse(request.get()));
		for (const char* errorField : { "Error Message", "Note", "Information" })
		{
			if (response.contains(errorField))
			{
				throw std::runtime_error("Alpha Vantage: " + response.at(errorField).get<std::string>());
			}
		}
		return response;
	}

	Json fetchApiResponse(std::string_view apiKey, std::string_view query)
	{
		for (unsigned attempt = 1; attempt <= maximumRequestAttempts; ++attempt)
		{
			try
			{
				return fetchApiResponseOnce(apiKey, query);
			}
			catch (const RetryableHttpStatus& error)
			{
				if (attempt == maximumRequestAttempts)
				{
					throw std::runtime_error(std::string(error.what()) + "; retry limit reached after " +
						std::to_string(maximumRequestAttempts) + " attempts");
				}
				std::this_thread::sleep_for(retryDelay(attempt));
			}
			catch (const std::system_error& error)
			{
				if (!isRetryableWinHttpError(error.code().value()))
				{
					throw;
				}
				if (attempt == maximumRequestAttempts)
				{
					throw std::system_error(error.code(), std::string(error.what()) +
						"; retry limit reached after " + std::to_string(maximumRequestAttempts) + " attempts");
				}
				std::this_thread::sleep_for(retryDelay(attempt));
			}
		}
		throw std::logic_error("Alpha Vantage retry loop exited unexpectedly");
	}
}

AlphaVantageClient::AlphaVantageClient(std::string apiKey)
	: apiKey_(std::move(apiKey))
{
	if (apiKey_.empty())
	{
		throw std::invalid_argument("ALPHAVANTAGE_API_KEY is missing. Set it in the app's environment "
			"(in Visual Studio: Project Properties > Configuration Properties > Debugging > Environment) "
			"and restart the app");
	}
}

StockPrice AlphaVantageClient::fetchGlobalQuote(std::string_view symbol) const
{
	const std::string normalizedSymbol = normalizeSymbol(symbol);
	const std::string query = "function=GLOBAL_QUOTE&symbol=" + urlEncode(normalizedSymbol);
	const Json response = fetchApiResponse(apiKey_, query);

	const auto quoteEntry = response.find("Global Quote");
	if (quoteEntry == response.end() || !quoteEntry->is_object() || quoteEntry->empty())
	{
		throw std::runtime_error("Alpha Vantage did not return a quote for " + normalizedSymbol);
	}
	const Json& quote = *quoteEntry;
	return StockPrice{
		normalizedSymbol,
		parseTradingDay(quote),
		parseDouble(quote, "02. open"),
		parseDouble(quote, "03. high"),
		parseDouble(quote, "04. low"),
		parseDouble(quote, "05. price"),
		AlphaVantageParsing::parseGlobalQuoteVolume(quote)
	};
}

std::vector<StockPrice> AlphaVantageClient::fetchDailyHistory(std::string_view symbol) const
{
	const std::string normalizedSymbol = normalizeSymbol(symbol);
	const std::string query = "function=TIME_SERIES_DAILY&symbol=" + urlEncode(normalizedSymbol) +
		"&outputsize=compact";
	const Json response = fetchApiResponse(apiKey_, query);
	const auto seriesEntry = response.find("Time Series (Daily)");
	if (seriesEntry == response.end() || !seriesEntry->is_object() || seriesEntry->empty())
	{
		throw std::runtime_error("Alpha Vantage did not return daily history for " + normalizedSymbol);
	}

	std::vector<StockPrice> history;
	history.reserve(seriesEntry->size());
	for (auto entry = seriesEntry->begin(); entry != seriesEntry->end(); ++entry)
	{
		if (!entry.value().is_object())
		{
			throw std::runtime_error("Unexpected Alpha Vantage daily history entry");
		}
		const Json& quote = entry.value();
		history.push_back(StockPrice{
			normalizedSymbol,
			parseTradingDate(entry.key()),
			parseDouble(quote, "1. open"),
			parseDouble(quote, "2. high"),
			parseDouble(quote, "3. low"),
			parseDouble(quote, "4. close"),
			AlphaVantageParsing::parseDailyVolume(quote)
		});
	}
	std::sort(history.begin(), history.end(), [](const StockPrice& left, const StockPrice& right)
		{
			return left.timestamp < right.timestamp;
		});
	return history;
}
