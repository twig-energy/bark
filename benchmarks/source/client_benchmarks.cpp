
#include <cstddef>
#include <vector>

#include "bark/client.hpp"

#include <benchmark/benchmark.h>

#include "./benchmark_helpers.hpp"
#include "bark/event.hpp"
#include "bark/gauge.hpp"
#include "bark/mpmc_client.hpp"
#include "bark/spsc_client.hpp"
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

constexpr auto number_of_iterations = std::size_t {25'000};

template<typename T, datagram_transport Transport>
auto create_client() -> T
{
    constexpr static auto queue_size = std::size_t {number_of_iterations};

    auto transport = create_transport<Transport>();

    if constexpr (std::is_same_v<T, Client<Transport>>) {
        return Client {std::move(transport)};
    } else if constexpr (std::is_same_v<T, SPSCClient>) {
        return SPSCClient {std::move(transport), queue_size};
    } else if constexpr (std::is_same_v<T, MPMCClient>) {
        return MPMCClient {std::move(transport), queue_size};
    }
}

template<typename T, datagram_transport Transport>
auto benchmark_client_send_metric(benchmark::State& state) -> void
{
    auto values = random_double_vector(1'000, 0.0, 1000.0);

    [[maybe_unused]]
    auto consumer = create_consumer<Transport>();
    auto client = create_client<T, Transport>();

    auto metrics = std::vector<Gauge>();
    metrics.reserve(number_of_iterations);
    for (auto i = std::size_t {0}; i < number_of_iterations; i++) {
        metrics.emplace_back(
            Gauge("metric_name", values[i % values.size()]).with(Tags::from_list({"tag1:hello", "tag2:world"})));
    }

    auto iteration = std::size_t {0};
    for (auto _ : state) {
        client.send(std::move(metrics[iteration]));
        iteration++;
    }
}

template<typename T, datagram_transport Transport>
auto benchmark_client_send_event(benchmark::State& state) -> void
{
    [[maybe_unused]]
    auto consumer = create_consumer<Transport>();
    auto client = create_client<T, Transport>();

    auto events = std::vector<Event>(number_of_iterations,
                                     Event("event", "text").with(Tags::from_list({"tag1:hello", "tag2:world"})));

    auto i = std::size_t {0};
    for (auto _ : state) {
        client.send(std::move(events[i]));
        i++;
    }
}

}  // namespace

BENCHMARK(benchmark_client_send_metric<Client<transports::UDPTransport>, transports::UDPTransport>)
    ->Iterations(number_of_iterations)
    ->Repetitions(32);
BENCHMARK(benchmark_client_send_metric<Client<transports::UDSTransport>, transports::UDSTransport>)
    ->Iterations(number_of_iterations)
    ->Repetitions(32);
BENCHMARK(benchmark_client_send_metric<Client<transports::AsyncUDPTransport>, transports::AsyncUDPTransport>)
    ->Iterations(number_of_iterations)
    ->Repetitions(32);
BENCHMARK(benchmark_client_send_metric<Client<transports::AsyncUDSTransport>, transports::AsyncUDSTransport>)
    ->Iterations(number_of_iterations)
    ->Repetitions(32);

BENCHMARK(benchmark_client_send_metric<SPSCClient, transports::UDPTransport>)
    ->Iterations(number_of_iterations)
    ->Repetitions(32);
BENCHMARK(benchmark_client_send_metric<SPSCClient, transports::UDSTransport>)
    ->Iterations(number_of_iterations)
    ->Repetitions(32);

BENCHMARK(benchmark_client_send_metric<MPMCClient, transports::UDPTransport>)
    ->Iterations(number_of_iterations)
    ->Repetitions(32);
BENCHMARK(benchmark_client_send_metric<MPMCClient, transports::UDSTransport>)
    ->Iterations(number_of_iterations)
    ->Repetitions(32);

BENCHMARK(benchmark_client_send_event<Client<transports::UDPTransport>, transports::UDPTransport>)
    ->Iterations(number_of_iterations)
    ->Repetitions(32);
BENCHMARK(benchmark_client_send_event<Client<transports::UDSTransport>, transports::UDSTransport>)
    ->Iterations(number_of_iterations)
    ->Repetitions(32);
BENCHMARK(benchmark_client_send_event<Client<transports::AsyncUDPTransport>, transports::AsyncUDPTransport>)
    ->Iterations(number_of_iterations)
    ->Repetitions(32);
BENCHMARK(benchmark_client_send_event<Client<transports::AsyncUDSTransport>, transports::AsyncUDSTransport>)
    ->Iterations(number_of_iterations)
    ->Repetitions(32);

BENCHMARK(benchmark_client_send_event<SPSCClient, transports::UDPTransport>)
    ->Iterations(number_of_iterations)
    ->Repetitions(32);
BENCHMARK(benchmark_client_send_event<SPSCClient, transports::UDSTransport>)
    ->Iterations(number_of_iterations)
    ->Repetitions(32);

BENCHMARK(benchmark_client_send_event<MPMCClient, transports::UDPTransport>)
    ->Iterations(number_of_iterations)
    ->Repetitions(32);
BENCHMARK(benchmark_client_send_event<MPMCClient, transports::UDSTransport>)
    ->Iterations(number_of_iterations)
    ->Repetitions(32);

}  // namespace bark
