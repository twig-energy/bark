#include <array>
#include <cstddef>
#include <string_view>
#include <utility>

#include <benchmark/benchmark.h>
#include <fmt/format.h>

#include "./benchmark_helpers.hpp"
#include "twig/datadog/count.hpp"
#include "twig/datadog/count_fmt.hpp"
#include "twig/datadog/gauge.hpp"
#include "twig/datadog/gauge_fmt.hpp"
#include "twig/datadog/histogram.hpp"
#include "twig/datadog/histogram_fmt.hpp"
#include "twig/datadog/tags.hpp"

namespace twig::datadog
{

namespace
{

template<typename T>
auto create_metric(std::size_t iteration) -> T
{
    static auto values = random_double_vector(100, 0.0, 1'000'000.0);
    static auto rates = random_double_vector(99, 0.0, 1.0);

    auto tags = Tags::create(std::array<std::string_view, 2> {"tag1:hello", "tag2:world"});

    if constexpr (std::is_same_v<T, Count>) {
        static auto values_integer = random_int32_t_vector(100, 0, 1'000'000);

        return T("count_metric", values_integer[iteration % values_integer.size()])
            .with_sample_rate(rates[iteration % rates.size()])
            .with_tags(std::move(tags));
    } else if constexpr (std::is_same_v<T, Gauge>) {
        return T("gauge_metric", values[iteration % values.size()]).with_tags(std::move(tags));
    } else if constexpr (std::is_same_v<T, Histogram>) {
        return T("histogram_metric", values[iteration % values.size()])
            .with_sample_rate(rates[iteration % rates.size()])
            .with_tags(std::move(tags));
    } else {
        static_assert(false, "Unsupported metric type");
    }
}

template<typename T>
auto benchmark_metric_serialize(benchmark::State& state) -> void
{
    auto iteration = std::size_t {0};
    for (auto _ : state) {
        auto metric = create_metric<T>(iteration);
        benchmark::DoNotOptimize(fmt::format("{}", metric));
        iteration++;
    }
}

}  // namespace

BENCHMARK(benchmark_metric_serialize<Count>);
BENCHMARK(benchmark_metric_serialize<Gauge>);
BENCHMARK(benchmark_metric_serialize<Histogram>);

}  // namespace twig::datadog
