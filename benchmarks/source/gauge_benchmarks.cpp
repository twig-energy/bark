#include <array>
#include <cstddef>
#include <string_view>
#include <utility>

#include "twig/datadog/gauge.hpp"

#include <benchmark/benchmark.h>
#include <fmt/format.h>

#include "./benchmark_helpers.hpp"
#include "twig/datadog/gauge_fmt.hpp"
#include "twig/datadog/tags.hpp"

namespace twig::datadog
{

namespace
{

auto benchmark_gauge_serialize(benchmark::State& state) -> void
{
    auto values = random_double_vector(100, 0.0, 1'000'000.0);

    auto iteration = std::size_t {0};
    for (auto _ : state) {
        auto tags = Tags::create(std::array<std::string_view, 2> {"tag1:hello", "tag2:world"});
        auto gauge = Gauge("metric_name", values[iteration % values.size()]).with_tags(std::move(tags));
        benchmark::DoNotOptimize(fmt::format("{}", gauge));
        iteration++;
    }
}

}  // namespace

BENCHMARK(benchmark_gauge_serialize);

}  // namespace twig::datadog
