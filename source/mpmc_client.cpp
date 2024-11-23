#include <cstddef>
#include <cstdint>
#include <utility>

#include "bark/mpmc_client.hpp"

#include "bark/datagram.hpp"
#include "bark/tags.hpp"
#include "bark/transports/udp_transport.hpp"

namespace bark
{

auto MPMCClient::send(const Datagram& datagram) -> void
{
    // NOTE: try_emplace means that the datagram will not be submitted if the queue is full.
    // unfortunately we have to copy datagram because std::variant is not nothrow copy constructible
    this->_queue->try_emplace(Datagram {datagram});
}

auto MPMCClient::send(Datagram&& datagram) -> void
{
    // NOTE: try_emplace means that the datagram will not be submitted if the queue is full.
    this->_queue->try_emplace(std::move(datagram));
}

auto MPMCClient::make_local_udp_client(std::size_t queue_size, Tags global_tags, uint16_t port) -> MPMCClient
{
    return {transports::UDPTransport::make_local_udp_transport(port, std::move(global_tags)), queue_size};
}

template MPMCClient::MPMCClient(transports::UDPTransport&&, std::size_t);
template MPMCClient::MPMCClient(transports::UDSTransport&&, std::size_t);

}  // namespace bark
