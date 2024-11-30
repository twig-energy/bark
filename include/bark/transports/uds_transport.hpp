#pragma once

#include <cstring>
#include <filesystem>
#include <memory>

#include "bark/asio_io_context_wrapper.hpp"
// ^ must be before asio includes, as it protects against gcc warnings
#include <asio/local/datagram_protocol.hpp>

#include "bark/datagram.hpp"
#include "bark/feature_detection.hpp"
#include "bark/tags.hpp"

#if BARK_UDS_ENABLED

namespace bark::transports
{

class UDSTransport
{
    Tags _global_tags;
    std::unique_ptr<asio::io_context> _io_context;
    std::unique_ptr<asio::local::datagram_protocol::endpoint> _endpoint;
    std::unique_ptr<asio::local::datagram_protocol::socket> _socket;

  public:
    explicit UDSTransport(const std::filesystem::path& socket_path, Tags global_tags = no_tags);

    auto send(Datagram&& datagram) -> bool;
};

}  // namespace bark::transports

#endif  // BARK_UDS_ENABLED
