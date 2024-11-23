#pragma once

#include <cstring>
#include <filesystem>
#include <memory>
#include <string_view>

#include "bark/asio_io_context_wrapper.hpp"
// ^ must be before asio includes, as it protects against gcc warnings

#include <asio/local/datagram_protocol.hpp>

namespace bark::transports
{

class UDSTransport
{
    std::unique_ptr<asio::io_context> _io_context;
    std::unique_ptr<asio::local::datagram_protocol::endpoint> _endpoint;
    std::unique_ptr<asio::local::datagram_protocol::socket> _socket;

  public:
    explicit UDSTransport(const std::filesystem::path& socket_path);

    auto send(std::string_view msg) -> bool;
};

}  // namespace bark::transports
