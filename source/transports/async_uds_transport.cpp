#include <cstdio>
#include <cstring>
#include <filesystem>
#include <memory>
#include <stdexcept>
#include <stop_token>
#include <utility>

#include "bark/transports/async_uds_transport.hpp"

#include "bark/asio_io_context_wrapper.hpp"
// ^ must be before asio includes, as it protects against gcc warnings

#include <asio/local/datagram_protocol.hpp>
#include <fmt/std.h>

#include "./async_transport_common.hpp"
#include "bark/datagram.hpp"
#include "bark/feature_detection.hpp"
#include "bark/number_of_io_threads.hpp"
#include "bark/tags.hpp"

#if BARK_UDS_ENABLED

namespace bark::transports
{

AsyncUDSTransport::AsyncUDSTransport(const std::filesystem::path& socket_path,
                                     NumberOfIOThreads num_io_threads,
                                     Tags global_tags)
    : _global_tags(std::make_unique<Tags>(std::move(global_tags)))
    , _io_context(std::make_unique<asio::io_context>(static_cast<int>(num_io_threads.value)))
    , _endpoint(std::make_unique<asio::local::datagram_protocol::endpoint>(socket_path.string()))
{
    if (num_io_threads.value == 0) {
        throw std::invalid_argument("Cannot have 0 IO threads on AsioClient");
    }

    this->_socket->open();
    this->_socket->connect(*this->_endpoint);

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

auto AsyncUDSTransport::send_async(Datagram&& datagram) -> void
{
    async_send(*this->_io_context, *this->_socket, *this->_global_tags, std::move(datagram));
}

}  // namespace bark::transports

#endif  // BARK_UDS_ENABLED
