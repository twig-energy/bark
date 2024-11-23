#include <cstddef>
#include <utility>
#include <vector>

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

const auto number_of_iterations = std::size_t {25000};

template<datagram_transport T>
auto benchmark_transport_send_metric(benchmark::State& state) -> void
{
    [[maybe_unused]]
    auto consumer = create_consumer<T>();
    auto transport = std::make_unique<T>(create_transport<T>());

    auto values = std::vector<Gauge>(number_of_iterations,
                                     Gauge("metric_name", 200.0).with(Tags::from_list({"tag1:hello", "tag2:world"})));

    auto i = std::size_t {0};
    for (auto a : state) {
        if constexpr (sync_datagram_transport<T>) {
            transport->send(std::move(values[i]));
        } else {
            transport->send_async(std::move(values[i]));
        }
        i++;
    }
}

}  // namespace

BENCHMARK(benchmark_transport_send_metric<transports::UDPTransport>)->Iterations(number_of_iterations)->Repetitions(16);
BENCHMARK(benchmark_transport_send_metric<transports::UDSTransport>)->Iterations(number_of_iterations)->Repetitions(16);
BENCHMARK(benchmark_transport_send_metric<transports::AsyncUDPTransport>)
    ->Iterations(number_of_iterations)
    ->Repetitions(16);
BENCHMARK(benchmark_transport_send_metric<transports::AsyncUDSTransport>)
    ->Iterations(number_of_iterations)
    ->Repetitions(16);

}  // namespace bark
