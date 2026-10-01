#include "../exercises/LabChecks.h"

#include <memory>
#include <vector>

/** @brief Interface for deterministic fee calculation. */
class FeePolicy {
public:
	virtual ~FeePolicy() = default;
	virtual double fee(double notional) const = 0;
};

class FlatFee : public FeePolicy {
public:
	double fee(double) const override { return 0.0; }
};

class RateFee : public FeePolicy {
public:
	explicit RateFee(double rate) : rate_(rate) {}
	double fee(double notional) const override { return notional * rate_; }
private:
	double rate_;
};

/** @brief Sums fees while retaining unique ownership of policy objects. */
double totalFees(const std::vector<std::unique_ptr<FeePolicy>>& policies, double notional) {
	double total = 0.0;
	for (const std::unique_ptr<FeePolicy>& policy : policies) {
		total += policy->fee(notional);
	}
	return total;
}

int main() {
	LabChecks checks;
	std::vector<std::unique_ptr<FeePolicy>> policies;
	policies.emplace_back(new FlatFee());
	policies.emplace_back(new RateFee(0.01));
	checks.expectEqual("polymorphic fees", 1.0, totalFees(policies, 100.0));
	checks.expectEqual("unique owners retain both policies", static_cast<std::size_t>(2), policies.size());
	return checks.finish("C++11 Polymorphism and Ownership");
}
