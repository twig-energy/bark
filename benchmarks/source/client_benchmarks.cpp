
#include <cstddef>
#include <cstdint>

#include "bark/client.hpp"

#include <benchmark/benchmark.h>

#include "./benchmark_helpers.hpp"
#include "bark/asio_client.hpp"
#include "bark/event.hpp"
#include "bark/gauge.hpp"
#include "bark/mpmc_client.hpp"
#include "bark/number_of_io_threads.hpp"
#include "bark/spsc_client.hpp"
#include "bark/tags.hpp"

namespace bark
{

namespace
{

constexpr auto number_of_iterations = std::size_t {25'000};

template<typename T>
auto create_client() -> T
{
    constexpr static auto port = int16_t {18125};
    constexpr static auto queue_size = std::size_t {number_of_iterations};

    if constexpr (std::is_same_v<T, Client>) {
        return Client::make_local_client(no_tags, port);
    } else if constexpr (std::is_same_v<T, AsioClient>) {
        return AsioClient::make_local_client(NumberOfIOThreads {1}, no_tags, port);
    } else if constexpr (std::is_same_v<T, SPSCClient>) {
        return SPSCClient::make_local_client(queue_size, no_tags, port);
    } else if constexpr (std::is_same_v<T, MPMCClient>) {
        return MPMCClient::make_local_client(queue_size, no_tags, port);
    }
}

template<typename T>
auto benchmark_client_send_metric(benchmark::State& state) -> void
{
    auto values = random_double_vector(1'000, 0.0, 1000.0);

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

BENCHMARK(benchmark_client_send_metric<Client>)->Iterations(number_of_iterations)->Repetitions(32);
BENCHMARK(benchmark_client_send_event<AsioClient>)->Iterations(number_of_iterations)->Repetitions(32);
BENCHMARK(benchmark_client_send_metric<SPSCClient>)->Iterations(number_of_iterations)->Repetitions(32);
BENCHMARK(benchmark_client_send_metric<MPMCClient>)->Iterations(number_of_iterations)->Repetitions(32);

BENCHMARK(benchmark_client_send_event<Client>)->Iterations(number_of_iterations)->Repetitions(32);
BENCHMARK(benchmark_client_send_metric<AsioClient>)->Iterations(number_of_iterations)->Repetitions(32);
BENCHMARK(benchmark_client_send_event<SPSCClient>)->Iterations(number_of_iterations)->Repetitions(32);
BENCHMARK(benchmark_client_send_event<MPMCClient>)->Iterations(number_of_iterations)->Repetitions(32);

}  // namespace bark
