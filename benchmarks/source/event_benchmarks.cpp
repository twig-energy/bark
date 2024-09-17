#include <utility>

#include "twig/datadog/event.hpp"

#include <benchmark/benchmark.h>

#include "twig/datadog/tags.hpp"

namespace twig::datadog
{

namespace
{

auto benchmark_event_serialize(benchmark::State& state) -> void
{
    for (auto _ : state) {
        auto tags = Tags::from_tags({"tag1:hello", "tag2:world"});

        auto event =
            Event("title",
                  "some long kind of description here - could probably also have some formatting if really wanted")
                .with_tags(std::move(tags))
                .with_alert_type(AlertType::ERROR);

        benchmark::DoNotOptimize(event.serialize());
    }
}

}  // namespace

BENCHMARK(benchmark_event_serialize);

}  // namespace twig::datadog
