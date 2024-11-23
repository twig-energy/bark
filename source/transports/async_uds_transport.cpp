#include <cstdio>
#include <cstring>
#include <filesystem>
#include <memory>
#include <source_location>
#include <stdexcept>
#include <stop_token>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>

#include "bark/transports/async_uds_transport.hpp"

#include "bark/asio_io_context_wrapper.hpp"
// ^ must be before asio includes, as it protects against gcc warnings
#include <asio/buffer.hpp>
#include <asio/local/datagram_protocol.hpp>
#include <asio/post.hpp>
#include <fmt/base.h>
#include <fmt/std.h>

#include "bark/number_of_io_threads.hpp"

namespace bark::transports
{

AsyncUDSTransport::AsyncUDSTransport(const std::filesystem::path& socket_path, NumberOfIOThreads num_io_threads)
    : _io_context(std::make_unique<asio::io_context>())
    , _socket(std::make_unique<asio::local::datagram_protocol::socket>(*this->_io_context))
{
    if (num_io_threads.value == 0) {
        throw std::invalid_argument("Cannot have 0 IO threads on AsioClient");
    }

    this->_socket->open();
    this->_socket->connect(socket_path.c_str());

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

auto AsyncUDSTransport::send_async(std::string_view msg) -> void
{
    this->send_async(std::string {msg});
}

auto AsyncUDSTransport::send_async(std::string&& msg) -> void
{
    asio::post(  //
        *this->_io_context,
        [socket_ptr = this->_socket.get(), message = std::move(msg)]() mutable
        {
            auto buffer = asio::buffer(message);

            socket_ptr->async_send(  //
                buffer,
                [msg = std::move(message)](const std::error_code& error, std::size_t)
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

auto AsyncUDSTransport::make_async_uds_transport(const std::filesystem::path& socket_path,
                                                 NumberOfIOThreads num_io_threads) -> AsyncUDSTransport
{
    return AsyncUDSTransport {socket_path, num_io_threads};
}

}  // namespace bark::transports
