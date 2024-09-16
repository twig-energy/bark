
#include <array>
#include <cstddef>
#include <string_view>
#include <utility>

#include "twig/datadog/spsc_client.hpp"

#include <asio/io_context.hpp>
#include <benchmark/benchmark.h>

#include "./benchmark_helpers.hpp"
#include "twig/datadog/client.hpp"
#include "twig/datadog/tags.hpp"
#include "twig/datadog/udp_client.hpp"

namespace twig::datadog
{

namespace
{

auto benchmark_spsc_client_send_async(benchmark::State& state) -> void
{
    auto values = random_double_vector(100, 0.0, 1'000'000.0);

    auto context = asio::io_context();
    auto client = SPSCClient {Client {UDPClient {context, "localhost", 18125}}, 1'000'000};

    auto iteration = std::size_t {0};
    for (auto _ : state) {
        auto tags = Tags::create(std::array<std::string_view, 2> {"tag1:hello", "tag2:world"});

        client.send_async(Gauge("metric_name", values[iteration % values.size()]).with_tags(std::move(tags)));
        iteration++;
    }
}

}  // namespace

BENCHMARK(benchmark_spsc_client_send_async)->Iterations(1'000'000);

}  // namespace twig::datadog
