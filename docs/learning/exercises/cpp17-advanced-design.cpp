#include "LabChecks.h"

#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <memory_resource>
#include <numeric>
#include <thread>
#include <vector>

/** @brief Runtime-polymorphic source of annual cash income. */
class IncomeSource {
public:
	virtual ~IncomeSource() = default;
	virtual int annualIncomeCents() const noexcept = 0;
};

/** @brief Equity position that pays a fixed per-share annual dividend. */
class DividendIncome final : public IncomeSource {
public:
	DividendIncome(int shares, int centsPerShare)
		: shares_(shares), centsPerShare_(centsPerShare) {}
	int annualIncomeCents() const noexcept override {
		return shares_ * centsPerShare_;
	}

private:
	int shares_;
	int centsPerShare_;
};

/** @brief Bond position that pays a fixed per-unit annual coupon. */
class CouponIncome final : public IncomeSource {
public:
	CouponIncome(int units, int centsPerUnit)
		: units_(units), centsPerUnit_(centsPerUnit) {}
	int annualIncomeCents() const noexcept override {
		return units_ * centsPerUnit_;
	}

private:
	int units_;
	int centsPerUnit_;
};

/** @brief Sums income through the base-class interface. */
std::int64_t totalAnnualIncome(const std::vector<std::unique_ptr<IncomeSource>>& sources) {
	std::int64_t total = 0;
	for (const auto& source : sources) {
		total += source->annualIncomeCents();
	}
	return total;
}

/** @brief A strong value type for a signed change measured in cents. */
struct PriceChange {
	int cents = 0;
};

/** @brief Adds two price changes; keep this non-member for symmetric syntax. */
PriceChange operator+(PriceChange left, PriceChange right) noexcept {
	return PriceChange{left.cents + right.cents};
}

/** @brief Compares two price changes by their represented cents. */
bool operator==(PriceChange left, PriceChange right) noexcept {
	return left.cents == right.cents;
}

/** @brief Reduces changes using the `PriceChange` addition operator. */
PriceChange netChange(const std::vector<PriceChange>& changes) {
	return std::accumulate(changes.begin(), changes.end(), PriceChange{});
}

/** @brief Copies a small batch into a local polymorphic-allocator vector and sums it. */
std::int64_t pooledVolume(const std::vector<int>& volumes) {
	alignas(std::max_align_t) std::array<std::byte, 64> storage{};
	std::pmr::monotonic_buffer_resource resource{
		storage.data(), storage.size(), std::pmr::null_memory_resource()
	};
	std::pmr::vector<int> localVolumes{&resource};
	localVolumes.reserve(volumes.size());
	localVolumes.insert(localVolumes.end(), volumes.begin(), volumes.end());
	return std::accumulate(localVolumes.begin(), localVolumes.end(), std::int64_t{0});
}

/** @brief Publishes a non-atomic value using a release/acquire flag. */
int publishedVolume() {
	std::atomic<bool> ready{false};
	int volume = 0;
	std::thread publisher([&] {
		volume = 1200;
		ready.store(true, std::memory_order_release);
	});

	for (unsigned attempt = 0; attempt < 100000 && !ready.load(std::memory_order_acquire); ++attempt) {
		std::this_thread::yield();
	}
	const bool wasPublished = ready.load(std::memory_order_acquire);
	publisher.join();
	return wasPublished ? volume : -1;
}

/** @brief Runs deterministic checks for polymorphism, operators, PMR, and publication. */
int main() {
	LabChecks checks;
	std::vector<std::unique_ptr<IncomeSource>> sources;
	sources.push_back(std::make_unique<DividendIncome>(120, 15));
	sources.push_back(std::make_unique<CouponIncome>(4, 75));
	checks.expectEqual("virtual dispatch sums mixed income sources", std::int64_t{2100}, totalAnnualIncome(sources));

	const std::vector<PriceChange> changes{{20}, {-5}, {15}};
	checks.expectEqual("non-member addition supports accumulation", true, netChange(changes) == PriceChange{30});
	checks.expectEqual("addition preserves an empty total", true, netChange({}) == PriceChange{});

	checks.expectEqual("PMR-backed volume total", std::int64_t{1000}, pooledVolume({100, 200, 700}));
	checks.expectEqual("release/acquire publishes the payload", 1200, publishedVolume());
	return checks.finish("C++17 Advanced Design");
}