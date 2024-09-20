
#include <cstddef>
#include <cstdint>

#include "twig/datadog/client.hpp"

#include <benchmark/benchmark.h>

#include "./benchmark_helpers.hpp"
#include "twig/datadog/asio_client.hpp"
#include "twig/datadog/event.hpp"
#include "twig/datadog/gauge.hpp"
#include "twig/datadog/mpmc_client.hpp"
#include "twig/datadog/spsc_client.hpp"
#include "twig/datadog/tags.hpp"

namespace twig::datadog
{

namespace
{

template<typename T>
auto create_client() -> T
{
    constexpr static auto port = int16_t {18125};
    constexpr static auto queue_size = std::size_t {1'000'000};

    if constexpr (std::is_same_v<T, Client>) {
        return Client::make_local_client(no_tags, port);
    } else if constexpr (std::is_same_v<T, AsioClient>) {
        return AsioClient::make_local_client(1, no_tags, port);
    } else if constexpr (std::is_same_v<T, SPSCClient>) {
        return SPSCClient::make_local_client(queue_size, no_tags, port);
    } else if constexpr (std::is_same_v<T, MPMCClient>) {
        return MPMCClient::make_local_client(queue_size, no_tags, port);
    }
}

template<typename T>
auto benchmark_client_send_metric(benchmark::State& state) -> void
{
    auto values = random_double_vector(100, 0.0, 1'000'000.0);

    auto client = create_client<T>();

    auto iteration = std::size_t {0};
    for (auto _ : state) {
        client.send(Gauge("metric_name", values[iteration % values.size()])
                        .with(Tags::from_list({"tag1:hello", "tag2:world"})));
        iteration++;
    }
}

template<typename T>
auto benchmark_client_send_event(benchmark::State& state) -> void
{
    auto client = create_client<T>();

    for (auto _ : state) {
        client.send(Event("event", "text").with(Tags::from_list({"tag1:hello", "tag2:world"})));
    }
}

}  // namespace

BENCHMARK(benchmark_client_send_metric<Client>);
BENCHMARK(benchmark_client_send_metric<AsioClient>);
BENCHMARK(benchmark_client_send_metric<SPSCClient>)->Iterations(1'000'000);
BENCHMARK(benchmark_client_send_metric<MPMCClient>)->Iterations(1'000'000);

BENCHMARK(benchmark_client_send_event<Client>);
BENCHMARK(benchmark_client_send_event<AsioClient>);
BENCHMARK(benchmark_client_send_event<SPSCClient>)->Iterations(1'000'000);
BENCHMARK(benchmark_client_send_event<MPMCClient>)->Iterations(1'000'000);

}  // namespace twig::datadog
