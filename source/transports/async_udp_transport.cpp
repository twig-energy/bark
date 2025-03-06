#include <cstdint>
#include <cstdio>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <stop_token>
#include <string>
#include <string_view>
#include <utility>

#include "bark/transports/async_udp_transport.hpp"

#include "bark/asio_io_context_wrapper.hpp"
// ^ must be before asio includes, as it protects against gcc warnings

#include <asio/ip/udp.hpp>
#include <fmt/std.h>

#include "./async_transport_common.hpp"
#include "bark/datagram.hpp"
#include "bark/number_of_io_threads.hpp"
#include "bark/tags.hpp"

namespace bark::transports
{

AsyncUDPTransport::AsyncUDPTransport(std::string_view host,
                                     uint16_t port,
                                     NumberOfIOThreads num_io_threads,
                                     Tags global_tags)
    : _global_tags(std::make_unique<Tags>(std::move(global_tags)))
    , _io_context(std::make_unique<asio::io_context>(static_cast<int>(num_io_threads.value)))
    , _receiver_endpoint(
          std::make_unique<asio::ip::udp::endpoint>(*asio::ip::udp::resolver(*this->_io_context)
                                                         .resolve(asio::ip::udp::v4(), host, std::to_string(port))
                                                         .begin()))
    , _socket(std::make_unique<asio::ip::udp::socket>(*this->_io_context))
{
    if (num_io_threads.value == 0) {
        throw std::invalid_argument("Cannot have 0 IO threads on AsioClient");
    }

    this->_socket->open(asio::ip::udp::v4());
    this->_socket->connect(*this->_receiver_endpoint);

    this->_io_threads.reserve(num_io_threads.value);
    for (auto i = 0ULL; i < num_io_threads.value; i++) {
        this->_io_threads.emplace_back(
            [io_context_ptr = this->_io_context.get()](const std::stop_token& stop_token)
            {
                while (!stop_token.stop_requested()) {
                    io_context_ptr->run();
                }
            });
    }
}

auto AsyncUDPTransport::send_async(Datagram&& datagram) -> void
{
    async_send(*this->_io_context, this->_socket, std::move(datagram), this->_global_tags);
}

auto AsyncUDPTransport::make_async_local_udp_transport(NumberOfIOThreads number_of_threads,
                                                       uint16_t port,
                                                       Tags global_tags) -> AsyncUDPTransport
{
    return {"localhost", port, number_of_threads, std::move(global_tags)};
}

}  // namespace bark::transports
