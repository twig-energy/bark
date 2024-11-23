#include <cstddef>
#include <cstdint>
#include <utility>

#include "bark/spsc_client.hpp"

#include "bark/datagram.hpp"
#include "bark/tags.hpp"
#include "bark/transports/udp_transport.hpp"

namespace bark
{

auto SPSCClient::send(Datagram&& datagram) -> void
{
    // NOTE: try_emplace means that the datagram will not be submitted if the queue is full.
    this->_queue->try_emplace(std::move(datagram));
}

auto SPSCClient::send(const Datagram& datagram) -> void
{
    // NOTE: try_emplace means that the datagram will not be submitted if the queue is full.
    this->_queue->try_emplace(datagram);
}

auto SPSCClient::make_local_udp_client(std::size_t queue_size, Tags global_tags, uint16_t port) -> SPSCClient
{
    return {transports::UDPTransport::make_local_udp_transport(port, std::move(global_tags)), queue_size};
}

template SPSCClient::SPSCClient(transports::UDPTransport&&, std::size_t);
template SPSCClient::SPSCClient(transports::UDSTransport&&, std::size_t);

}  // namespace bark
