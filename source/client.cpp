#include <bark/client.hpp>

#include "bark/transports/async_udp_transport.hpp"
#include "bark/transports/async_uds_transport.hpp"
#include "bark/transports/udp_transport.hpp"
#include "bark/transports/uds_transport.hpp"

namespace bark
{

template class Client<transports::AsyncUDPTransport>;
template class Client<transports::AsyncUDSTransport>;
template class Client<transports::UDPTransport>;
template class Client<transports::UDSTransport>;

}  // namespace bark
