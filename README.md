# EquityLens

EquityLens is a C++23 console stock analyzer and a progressive C++ learning project. It fetches Alpha Vantage quotes and daily history, stores OHLCV observations in SQLite, calculates summary statistics and technical indicators, renders ASCII candlestick charts, and exports saved history to CSV.

## Build

Open `EquityLens.slnx` in Visual Studio with vcpkg integration enabled. The root `vcpkg.json` manifest supplies SQLite and JSON dependencies. Build the `EquityLens` project with the x64 configuration.

## Tests

Build `EquityLens.Tests` with the x64 configuration, then run `x64/Debug/EquityLens.Tests.exe` from the solution directory. The offline test runner covers summary and technical indicators, chart/CSV presentation, and SQLite persistence, range queries, upserts, and validation. It does not make live API requests.

## Run

Set `ALPHAVANTAGE_API_KEY` in the app's environment. In Visual Studio, use **Project Properties > Configuration Properties > Debugging > Environment** and add `ALPHAVANTAGE_API_KEY=<your-key>`. Keep the key private; do not commit it or include it in bug reports. The available commands are:

- `EquityLens.exe MSFT AAPL` — fetch and store the latest quote for each symbol.
- `EquityLens.exe history AAPL` — fetch Alpha Vantage's compact daily series (up to 100 observations), save it, and display saved history.
- `EquityLens.exe stats AAPL` — summarize all saved observations for the symbol; this command is offline. Run `history` first to load provider history.
- `EquityLens.exe indicators AAPL` — show the latest 60 saved closes with SMA(14), Wilder RSI(14), and 20-day Bollinger Bands (two population standard deviations); warm-up values are shown as `-`.
- `EquityLens.exe chart AAPL` — render the latest 60 saved observations as ASCII candlesticks; `#` is up, `o` is down, `=` is unchanged, and `|` is the high/low wick.
- `EquityLens.exe export AAPL AAPL.csv` — export saved OHLCV history as escaped, locale-independent CSV.
- `EquityLens.exe` — enter symbols one per line and submit a blank line when finished.
- `EquityLens.exe --help` — display command usage.

Requests are spaced by at least 1.1 seconds to comply with Alpha Vantage's per-second limit. Transient network failures and selected temporary HTTP statuses receive at most two retries with exponential backoff; HTTP 429 and provider quota messages are reported without automatic retries. Daily request limits and plan-specific history availability still apply. Data provider terms are separate from this project's MIT license; review them before redistributing provider data. The existing `stock_market.db` filename is retained to preserve saved data.

## Learn C++ progressively

The final application remains C++23. The [learning path](docs/learning/README.md) provides separate lessons, explanations, and exercises for:

1. C++11 — RAII, smart pointers, and lambdas.
2. C++14 — `std::make_unique` and generic lambdas.
3. C++17 — `std::optional`, `std::variant`, structured bindings, and `std::filesystem`.
4. C++20 — concepts, ranges, coroutines, and `std::jthread`.
5. C++23 — `std::expected` and `std::mdspan`.
6. C++26 — support-aware exploration of evolving language and library facilities.

The lessons connect the language features to this application's code and explain where a simpler design is preferable.

## License

Project source is licensed under MIT; see [LICENSE](LICENSE). This does not grant rights to Alpha Vantage data.
