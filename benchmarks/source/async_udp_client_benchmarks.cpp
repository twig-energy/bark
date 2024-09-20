
#include <cstddef>

#include "twig/datadog/async_udp_client.hpp"

#include <benchmark/benchmark.h>

#include "./benchmark_helpers.hpp"
#include "twig/datadog/gauge.hpp"
#include "twig/datadog/tags.hpp"

namespace twig::datadog
{

namespace
{

auto benchmark_async_udp_client_send_metric(benchmark::State& state) -> void
{
    auto client = AsyncUDPClient::make_local_udp_client(2, 12347);

    const auto value =
        Gauge("metric_name", 200.0).with(Tags::from_list({"tag1:hello", "tag2:world"})).serialize(no_tags);

    for (auto _ : state) {
        client.send(value);
    }
}

}  // namespace

BENCHMARK(benchmark_async_udp_client_send_metric)->Iterations(1'000'000);

}  // namespace twig::datadog
