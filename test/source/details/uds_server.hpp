#pragma once
#include <cstddef>
#include <filesystem>
#include <functional>
#include <string_view>
#include <system_error>
#include <vector>

#include "bark/asio_io_context_wrapper.hpp"
// ^ must be before asio includes, as it protects against gcc warnings

#include <asio/local/datagram_protocol.hpp>
#include <fmt/core.h>

#include "bark/feature_detection.hpp"

#if BARK_UDS_ENABLED

namespace bark
{

class UDSServer
{
    asio::local::datagram_protocol::endpoint _endpoint;
    asio::local::datagram_protocol::socket _socket;
    std::function<void(std::string_view)> _receive_msg_callback;
    std::vector<char> _recv_buffer;
    asio::local::datagram_protocol::endpoint _latest_senders_endpoint;

  public:
    UDSServer(asio::io_context& io_context,
              const std::filesystem::path& socket_path,
              std::function<void(std::string_view)> receive_msg_callback,
              size_t buffer_size);

    ~UDSServer();

  private:
    auto start_receive() -> void;

    auto handle_receive(const std::error_code& error, std::size_t bytes_transferred) -> void;
};

}  // namespace bark

#endif  // BARK_UDS_ENABLED
