#include <cstddef>
#include <cstdio>
#include <filesystem>
#include <functional>
#include <source_location>
#include <string_view>
#include <system_error>
#include <utility>

#include "bark/asio_io_context_wrapper.hpp"
// ^ must be before asio includes, as it protects against gcc warnings

#include <asio/buffer.hpp>
#include <asio/local/datagram_protocol.hpp>
#include <fmt/base.h>
#include <fmt/std.h>

#include "./uds_server.hpp"

namespace bark
{

UDSServer::UDSServer(asio::io_context& io_context,
                     const std::filesystem::path& socket_path,
                     std::function<void(std::string_view)> receive_msg_callback,
                     size_t buffer_size)
    : _endpoint(socket_path.c_str())
    , _socket(io_context, this->_endpoint)
    , _receive_msg_callback(std::move(receive_msg_callback))
    , _recv_buffer(buffer_size)
{
    start_receive();
}

UDSServer::~UDSServer()
{
    this->_socket.close();
    std::filesystem::remove(this->_endpoint.path());
}

auto UDSServer::start_receive() -> void
{
    this->_socket.async_receive_from(asio::buffer(_recv_buffer),
                                     this->_latest_senders_endpoint,
                                     [this](const std::error_code& error, std::size_t bytes_transferred)
                                     { handle_receive(error, bytes_transferred); });
}

auto UDSServer::handle_receive(const std::error_code& error, std::size_t bytes_transferred) -> void
{
    // UDP we don't care about errors.
    if (!error) [[likely]] {
        this->_receive_msg_callback(std::string_view(this->_recv_buffer.data(), bytes_transferred));
    } else {
        fmt::println(stderr, "Failed at receiving, maybe buffer is too small? {}", std::source_location::current());
    }
    start_receive();
}

}  // namespace bark
