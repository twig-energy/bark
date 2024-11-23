#pragma once

#include <cstdint>

#include "bark/datagram.hpp"
#include "bark/i_datadog_client.hpp"
#include "bark/tags.hpp"
#include "bark/transports/async_udp_transport.hpp"
#include "bark/transports/async_uds_transport.hpp"
#include "bark/transports/constants.hpp"
#include "bark/transports/datagram_transport.hpp"
#include "bark/transports/udp_transport.hpp"
#include "bark/transports/uds_transport.hpp"

namespace bark
{

template<datagram_transport Transport>
class Client final : public IDatadogClient
{
    Transport _transport;

  public:
    explicit Client(Transport&& transport)
        : _transport(std::move(transport))
    {
    }

    auto send(const Datagram& datagram) -> void override
    {
        if constexpr (sync_datagram_transport<Transport>) {
            this->_transport.send(Datagram {datagram});
        } else {
            this->_transport.send_async(Datagram {datagram});
        }
    }

    auto send(Datagram&& datagram) -> void override
    {
        if constexpr (sync_datagram_transport<Transport>) {
            this->_transport.send(std::move(datagram));
        } else {
            this->_transport.send_async(std::move(datagram));
        }
    }
};

using UDPClient = Client<transports::UDPTransport>;

inline auto make_local_udp_client(Tags global_tags = no_tags,
                                  uint16_t port = transports::dogstatsd_udp_port) -> UDPClient
{
    return UDPClient {transports::UDPTransport::make_local_udp_transport(port, std::move(global_tags))};
}

extern template class Client<transports::AsyncUDPTransport>;
extern template class Client<transports::AsyncUDSTransport>;
extern template class Client<transports::UDPTransport>;
extern template class Client<transports::UDSTransport>;

}  // namespace bark
