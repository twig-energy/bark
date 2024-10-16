#pragma once
#include <cstddef>
#include <cstdint>
#include <functional>
#include <string_view>
#include <system_error>
#include <vector>

#include <asio/io_context.hpp>
#include <asio/ip/udp.hpp>
#include <fmt/core.h>

namespace bark
{

class UDPServer
{
    asio::ip::udp::socket _socket;
    std::function<void(std::string_view)> _receive_msg_callback;
    std::vector<char> _recv_buffer;
    asio::ip::udp::endpoint _latest_senders_endpoint;

  public:
    UDPServer(asio::io_context& io_context,
              int32_t port,
              std::function<void(std::string_view)> receive_msg_callback,
              size_t buffer_size);

  private:
    auto start_receive() -> void;

    auto handle_receive(const std::error_code& error, std::size_t bytes_transferred) -> void;
};

}  // namespace bark
