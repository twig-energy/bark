
#include <array>
#include <cstddef>
#include <string_view>
#include <utility>

#include "twig/datadog/client.hpp"

#include <asio/io_context.hpp>
#include <benchmark/benchmark.h>

#include "./benchmark_helpers.hpp"
#include "twig/datadog/tags.hpp"
#include "twig/datadog/udp_client.hpp"

namespace twig::datadog
{

namespace
{

auto benchmark_client_send_async(benchmark::State& state) -> void
{
    auto values = random_double_vector(100, 0.0, 1'000'000.0);

    auto context = asio::io_context();
    auto client = Client(UDPClient(context, "localhost", 18125));

    auto iteration = std::size_t {0};
    for (auto _ : state) {
        auto tags = Tags::create(std::array<std::string_view, 2> {"tag1:hello", "tag2:world"});
        auto gauge = Gauge("metric_name", values[iteration % values.size()]).with_tags(std::move(tags));

        client.send_async(gauge);
        iteration++;
    }
}

}  // namespace

BENCHMARK(benchmark_client_send_async);

}  // namespace twig::datadog
