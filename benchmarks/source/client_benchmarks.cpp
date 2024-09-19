
#include <cstddef>
#include <cstdint>
#include <string_view>
#include <utility>

#include "twig/datadog/client.hpp"

#include <benchmark/benchmark.h>

#include "./benchmark_helpers.hpp"
#include "twig/datadog/event.hpp"
#include "twig/datadog/gauge.hpp"
#include "twig/datadog/mpmc_client.hpp"
#include "twig/datadog/spsc_client.hpp"
#include "twig/datadog/tags.hpp"
#include "twig/datadog/udp_client.hpp"

namespace twig::datadog
{

namespace
{

template<typename T>
auto create_client() -> T
{
    constexpr static auto hostname = std::string_view {"localhost"};
    constexpr static auto port = int16_t {18125};
    constexpr static auto queue_size = std::size_t {1'000'000};

    if constexpr (std::is_same_v<T, Client>) {
        return Client(UDPClient(hostname, port));
    } else if constexpr (std::is_same_v<T, SPSCClient>) {
        return SPSCClient(UDPClient(hostname, port), queue_size);
    } else if constexpr (std::is_same_v<T, MPMCClient>) {
        return MPMCClient(UDPClient(hostname, port), queue_size);
    }
}

template<typename T>
auto benchmark_client_send_metric(benchmark::State& state) -> void
{
    auto values = random_double_vector(100, 0.0, 1'000'000.0);

    auto client = create_client<T>();

    auto iteration = std::size_t {0};
    for (auto _ : state) {
        auto tags = Tags::from_list({"tag1:hello", "tag2:world"});

        client.send(std::move(Gauge("metric_name", values[iteration % values.size()]).with(std::move(tags))));
        iteration++;
    }
}

template<typename T>
auto benchmark_client_send_event(benchmark::State& state) -> void
{
    auto client = create_client<T>();

    for (auto _ : state) {
        auto tags = Tags::from_list({"tag1:hello", "tag2:world"});

        client.send(std::move(Event("event", "text").with(std::move(tags))));
    }
}

}  // namespace

BENCHMARK(benchmark_client_send_metric<Client>);
BENCHMARK(benchmark_client_send_metric<SPSCClient>)->Iterations(1'000'000);
BENCHMARK(benchmark_client_send_metric<MPMCClient>)->Iterations(1'000'000);

BENCHMARK(benchmark_client_send_event<Client>);
BENCHMARK(benchmark_client_send_event<SPSCClient>)->Iterations(1'000'000);
BENCHMARK(benchmark_client_send_event<MPMCClient>)->Iterations(1'000'000);

}  // namespace twig::datadog
