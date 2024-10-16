#include <utility>

#include "bark/event.hpp"

#include <benchmark/benchmark.h>

#include "bark/tags.hpp"

namespace bark
{

namespace
{

auto benchmark_event_serialize(benchmark::State& state) -> void
{
    for (auto _ : state) {
        auto tags = Tags::from_list({"tag1:hello", "tag2:world"});

        auto event =
            Event("title",
                  "some long kind of description here - could probably also have some formatting if really wanted")
                .with(std::move(tags))
                .with(AlertType::ERR);

        benchmark::DoNotOptimize(event.serialize(no_tags));
    }
}

}  // namespace

BENCHMARK(benchmark_event_serialize)->Repetitions(16);

}  // namespace bark
