#include <random>

namespace pong {

template <
          typename IntType = int,
          typename EngineType = std::minstd_rand,
          typename Distribution = typename std::uniform_int_distribution<IntType>
         >
class Rand {
public:
    Rand(
            IntType min = std::numeric_limits<IntType>::min(),
            IntType max = std::numeric_limits<IntType>::max())
    {
        std::random_device rd;
        engine_ = EngineType(rd());
        distribution_ = Distribution(min, max);
    }

    int get() {
        return distribution_(engine_);
    }

private:
    EngineType engine_;
    Distribution distribution_;
};

} // namespace pong
