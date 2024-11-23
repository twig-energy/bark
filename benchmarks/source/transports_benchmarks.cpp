
#include <benchmark/benchmark.h>

#include "./benchmark_helpers.hpp"
#include "bark/gauge.hpp"
#include "bark/tags.hpp"
#include "bark/transports/async_udp_transport.hpp"
#include "bark/transports/async_uds_transport.hpp"
#include "bark/transports/datagram_transport.hpp"
#include "bark/transports/udp_transport.hpp"
#include "bark/transports/uds_transport.hpp"

namespace bark
{

namespace
{

template<datagram_transport T>
auto benchmark_transport_send_metric(benchmark::State& state) -> void
{
    [[maybe_unused]]
    auto consumer = create_consumer<T>();
    auto transport = std::make_unique<T>(create_transport<T>());

    const auto value = Gauge("metric_name", 200.0).with(Tags::from_list({"tag1:hello", "tag2:world"}));

    for (auto _ : state) {
        if constexpr (sync_datagram_transport<T>) {
            transport->send(value);
        } else {
            transport->send_async(value);
        }
    }
}

}  // namespace

BENCHMARK(benchmark_transport_send_metric<transports::UDPTransport>)->Iterations(10000)->Repetitions(16);
BENCHMARK(benchmark_transport_send_metric<transports::UDSTransport>)->Iterations(10000)->Repetitions(16);
BENCHMARK(benchmark_transport_send_metric<transports::AsyncUDPTransport>)->Iterations(10000)->Repetitions(16);
BENCHMARK(benchmark_transport_send_metric<transports::AsyncUDSTransport>)->Iterations(10000)->Repetitions(16);

}  // namespace bark
