#include <cstdint>
#include <cstdio>
#include <cstring>
#include <memory>
#include <source_location>
#include <stdexcept>
#include <stop_token>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <variant>
#include <vector>

#include "twig/datadog/asio_client.hpp"

#include <asio/buffer.hpp>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnull-dereference"
#include <asio/io_context.hpp>
#pragma GCC diagnostic pop
#include <asio/ip/udp.hpp>
#include <asio/post.hpp>
#include <fmt/base.h>
#include <fmt/std.h>

#include "twig/datadog/datagram.hpp"
#include "twig/datadog/number_of_io_threads.hpp"
#include "twig/datadog/tags.hpp"

namespace twig::datadog
{

AsioClient::AsioClient(std::string_view host, uint16_t port, NumberOfIOThreads num_io_threads, Tags global_tags)
    : _global_tags(std::make_unique<Tags>(std::move(global_tags)))
    , _io_context(std::make_unique<asio::io_context>(num_io_threads.value))
    , _receiver_endpoint(
          std::make_unique<asio::ip::udp::endpoint>(*asio::ip::udp::resolver(*this->_io_context)
                                                         .resolve(asio::ip::udp::v4(), host, std::to_string(port))
                                                         .begin()))
{
    if (num_io_threads.value == 0) {
        throw std::invalid_argument("Cannot have 0 IO threads on AsioClient");
    }

    this->_socket->open(asio::ip::udp::v4());

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

auto AsioClient::send(const Datagram& datagram) -> void
{
    this->send(auto(datagram));
}

auto AsioClient::send(Datagram&& datagram) -> void
{
    asio::post(  //
        *this->_io_context,
        [global_tags_ptr = this->_global_tags.get(),
         socket_ptr = this->_socket.get(),
         receiver_endpoint_ptr = this->_receiver_endpoint.get(),
         message = std::move(datagram)]() mutable
        {
            auto serialized = std::visit([global_tags_ptr](const auto& serializable_datagram)
                                         { return serializable_datagram.serialize(*global_tags_ptr); },
                                         message);

            auto buffer = asio::buffer(serialized);

            socket_ptr->async_send_to(  //
                buffer,
                *receiver_endpoint_ptr,
                [msg = std::move(serialized)](const std::error_code& error, std::size_t)
                {
                    // Move in `serialized`, to make the callback clean up, such that we are sure we are not using a
                    // cleaned up string.
                    if (error) [[unlikely]] {
                        fmt::println(
                            stderr, "Failed at sending {}. {}", error.message(), std::source_location::current());
                    }
                });
        });
}

auto AsioClient::make_local_client(NumberOfIOThreads num_io_threads, Tags global_tags, uint16_t port) -> AsioClient
{
    return {"localhost", port, num_io_threads, std::move(global_tags)};
}

}  // namespace twig::datadog
