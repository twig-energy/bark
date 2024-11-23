#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <random>
#include <string_view>
#include <vector>

#include "../../test/source/details/raii_async_context.hpp"
#include "bark/number_of_io_threads.hpp"
#include "bark/transports/async_udp_transport.hpp"
#include "bark/transports/async_uds_transport.hpp"
#include "bark/transports/datagram_transport.hpp"
#include "bark/transports/udp_transport.hpp"
#include "bark/transports/uds_transport.hpp"

namespace bark
{

namespace
{

static auto rd = std::random_device();
static auto gen = std::mt19937(rd());

}  // namespace

inline auto random_double_vector(std::size_t elements, double min, double max) -> std::vector<double>
{
    auto values = std::vector<double>(elements);
    auto dis = std::uniform_real_distribution<>(min, max);
    std::ranges::generate(values, [&]() { return dis(gen); });

    return values;
}

inline auto random_int32_t_vector(std::size_t elements, int32_t min, int32_t max) -> std::vector<int32_t>
{
    auto values = std::vector<int32_t>(elements);
    auto dis = std::uniform_int_distribution<int32_t>(min, max);
    std::ranges::generate(values, [&]() { return dis(gen); });

    return values;
}

inline const std::filesystem::path benchmark_uds_socket_path {"/tmp/bark_benchmarks.sock"};
inline const auto benchmark_port = int16_t {18125};

template<datagram_transport Transport>
auto create_consumer()
{
    if constexpr (std::is_same_v<Transport, transports::UDSTransport>
                  || std::is_same_v<Transport, transports::AsyncUDSTransport>)
    {
        return bark::make_uds_server(benchmark_uds_socket_path, [](std::string_view) {});
    } else {
        return bark::make_local_udp_server(benchmark_port, [](std::string_view) {});
    }
}

template<datagram_transport Transport>
auto create_transport() -> Transport
{
    if constexpr (std::is_same_v<Transport, transports::UDPTransport>) {
        return transports::UDPTransport::make_local_udp_transport(benchmark_port);
    } else if constexpr (std::is_same_v<Transport, transports::AsyncUDPTransport>) {
        return transports::AsyncUDPTransport::make_async_local_udp_transport(NumberOfIOThreads {1}, benchmark_port);
    } else if constexpr (std::is_same_v<Transport, transports::UDSTransport>) {
        return transports::UDSTransport {benchmark_uds_socket_path};
    } else if constexpr (std::is_same_v<Transport, transports::AsyncUDSTransport>) {
        return transports::AsyncUDSTransport::make_async_uds_transport(benchmark_uds_socket_path,
                                                                       NumberOfIOThreads {1});
    }
}

}  // namespace bark
