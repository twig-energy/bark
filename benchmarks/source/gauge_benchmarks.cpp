#include <algorithm>
#include <array>
#include <cstddef>
#include <random>
#include <string_view>
#include <vector>

#include "twig/datadog/gauge.hpp"

#include <benchmark/benchmark.h>
#include <fmt/format.h>

#include "twig/datadog/compile_time_string.hpp"
#include "twig/datadog/compile_time_string_fmt.hpp"  // NOLINT
#include "twig/datadog/gauge_fmt.hpp"                // NOLINT

namespace twig::datadog
{

namespace
{

auto random_double_vector(std::size_t elements, double min, double max) -> std::vector<double>
{
    static auto rd = std::random_device();
    static auto gen = std::mt19937(rd());

    auto values = std::vector<double>(elements);
    auto dis = std::uniform_real_distribution<>(min, max);
    std::generate(values.begin(), values.end(), [&]() { return dis(gen); });

    return values;
}

auto benchmark_serialize_gauge(benchmark::State& state) -> void
{
    auto values = random_double_vector(100, 0.0, 1'000'000.0);
    auto rates = random_double_vector(99, 0.0, 1.0);
    auto tags = std::array<CompileTimeString, 2> {"tag1:hello", "tag2:world"};

    auto iteration = std::size_t {0};
    for (auto _ : state) {
        auto gauge = Gauge("metric_name", values[iteration % values.size()], rates[iteration % rates.size()], tags);
        benchmark::DoNotOptimize(fmt::format("{}", gauge));
        iteration++;
    }
}

}  // namespace

BENCHMARK(benchmark_serialize_gauge);

}  // namespace twig::datadog
