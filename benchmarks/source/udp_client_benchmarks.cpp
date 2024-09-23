

#include "bark/udp_client.hpp"

#include <benchmark/benchmark.h>

#include "bark/gauge.hpp"
#include "bark/tags.hpp"

namespace bark
{

namespace
{

auto benchmark_udp_client_send_metric(benchmark::State& state) -> void
{
    auto client = UDPClient::make_local_udp_client(12347);

    const auto value =
        Gauge("metric_name", 200.0).with(Tags::from_list({"tag1:hello", "tag2:world"})).serialize(no_tags);

    for (auto _ : state) {
        client.send(value);
    }
}

}  // namespace

BENCHMARK(benchmark_udp_client_send_metric);

}  // namespace bark
