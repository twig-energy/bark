#include <array>
#include <cstddef>
#include <string_view>
#include <utility>

#include "twig/datadog/count.hpp"

#include <benchmark/benchmark.h>
#include <fmt/format.h>

#include "./benchmark_helpers.hpp"
#include "twig/datadog/count_fmt.hpp"
#include "twig/datadog/tags.hpp"

namespace twig::datadog
{

namespace
{

auto benchmark_count_serialize(benchmark::State& state) -> void
{
    auto values = random_int32_t_vector(100, 0, 1'000'000);

    auto iteration = std::size_t {0};
    for (auto _ : state) {
        auto tags = Tags::create(std::array<std::string_view, 2> {"tag1:hello", "tag2:world"});
        auto count = Count("metric_name", values[iteration % values.size()]).with_tags(std::move(tags));
        benchmark::DoNotOptimize(fmt::format("{}", count));
        iteration++;
    }
}

}  // namespace

BENCHMARK(benchmark_count_serialize);

}  // namespace twig::datadog
