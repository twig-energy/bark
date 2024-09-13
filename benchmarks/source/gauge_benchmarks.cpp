#include <array>
#include <string_view>

#include "twig/datadog/gauge.hpp"

#include <benchmark/benchmark.h>
#include <fmt/format.h>

#include "twig/datadog/gauge_fmt.hpp"  // NOLINT

namespace twig::datadog
{

auto BM_serialize_gauge(benchmark::State& state) -> void
{
    auto tags = std::array<std::string_view, 2> {"tag1:hello", "tag2:world"};
    auto gauge = Gauge("metric_name", 42.0, 0.5, tags);
    for (auto _ : state) {
        benchmark::DoNotOptimize(fmt::format("{}", gauge));
    }
}

BENCHMARK(BM_serialize_gauge);

}  // namespace twig::datadog
