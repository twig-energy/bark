
#include <cstddef>
#include <cstdint>
#include <string_view>
#include <utility>

#include "twig/datadog/client.hpp"

#include <asio/io_context.hpp>
#include <benchmark/benchmark.h>

#include "./benchmark_helpers.hpp"
#include "twig/datadog/mpmc_client.hpp"
#include "twig/datadog/spsc_client.hpp"
#include "twig/datadog/tags.hpp"
#include "twig/datadog/udp_client.hpp"

namespace twig::datadog
{

namespace
{

template<typename T>
auto create_client(asio::io_context& context) -> T
{
    constexpr static auto hostname = std::string_view {"localhost"};
    constexpr static auto port = int16_t {18125};
    constexpr static auto queue_size = std::size_t {1'000'000};

    if constexpr (std::is_same_v<T, Client>) {
        return Client(UDPClient(context, hostname, port));
    } else if constexpr (std::is_same_v<T, SPSCClient>) {
        return SPSCClient(Client(UDPClient(context, hostname, port)), queue_size);
    } else if constexpr (std::is_same_v<T, MPMCClient>) {
        return MPMCClient(Client(UDPClient(context, hostname, port)), queue_size);
    }
}

template<typename T>
auto benchmark_client_send_async(benchmark::State& state) -> void
{
    auto values = random_double_vector(100, 0.0, 1'000'000.0);

    auto context = asio::io_context();
    auto client = create_client<T>(context);

    auto iteration = std::size_t {0};
    for (auto _ : state) {
        auto tags = Tags::from_tags({"tag1:hello", "tag2:world"});

        client.send_async(
            std::move(Gauge("metric_name", values[iteration % values.size()]).with_tags(std::move(tags))));
        iteration++;
    }
}

}  // namespace

BENCHMARK(benchmark_client_send_async<Client>);
BENCHMARK(benchmark_client_send_async<SPSCClient>)->Iterations(1'000'000);
BENCHMARK(benchmark_client_send_async<MPMCClient>)->Iterations(1'000'000);

}  // namespace twig::datadog
