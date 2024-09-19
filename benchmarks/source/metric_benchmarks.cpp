#include <cstddef>
#include <utility>

#include <benchmark/benchmark.h>

#include "./benchmark_helpers.hpp"
#include "twig/datadog/count.hpp"
#include "twig/datadog/gauge.hpp"
#include "twig/datadog/histogram.hpp"
#include "twig/datadog/sample_rate.hpp"
#include "twig/datadog/tags.hpp"

namespace twig::datadog
{

namespace
{

template<typename T>
auto create_metric(std::size_t iteration) -> T
{
    auto tags = Tags::from_tags({"tag1:hello", "tag2:world"});

    if constexpr (std::is_same_v<T, Count>) {
        static const auto values = random_int32_t_vector(100, 0, 1'000'000);
        static const auto rates = random_double_vector(99, 0.0, 1.0);

        return T("count_metric", values[iteration % values.size()])
            .with(SampleRate {rates[iteration % rates.size()]})
            .with(std::move(tags));
    } else if constexpr (std::is_same_v<T, Gauge>) {
        static const auto values = random_double_vector(100, 0.0, 1'000'000.0);

        return T("gauge_metric", values[iteration % values.size()]).with(std::move(tags));
    } else if constexpr (std::is_same_v<T, Histogram>) {
        static const auto values = random_double_vector(100, 0.0, 1'000'000.0);
        static const auto rates = random_double_vector(99, 0.0, 1.0);

        return T("histogram_metric", values[iteration % values.size()])
            .with(SampleRate {rates[iteration % rates.size()]})
            .with(std::move(tags));
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
        benchmark::DoNotOptimize(metric.serialize(no_tags));
        iteration++;
    }
}

}  // namespace

BENCHMARK(benchmark_metric_serialize<Count>);
BENCHMARK(benchmark_metric_serialize<Gauge>);
BENCHMARK(benchmark_metric_serialize<Histogram>);

}  // namespace twig::datadog
