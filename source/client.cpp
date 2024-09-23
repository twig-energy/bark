#include <cstdint>
#include <utility>
#include <variant>

#include "bark/client.hpp"

#include "bark/datagram.hpp"
#include "bark/tags.hpp"
#include "bark/udp_client.hpp"

namespace bark
{

Client::Client(UDPClient&& udp_client, Tags global_tags)
    : _udp_client(std::move(udp_client))
    , _global_tags(std::move(global_tags))
{
}

auto Client::send(const Datagram& datagram) -> void
{
    this->_udp_client.send(std::visit([this](const auto& serializable_datagram)
                                      { return serializable_datagram.serialize(this->_global_tags); },
                                      datagram));
}

auto Client::send(Datagram&& datagram) -> void
{
    this->_udp_client.send(std::visit([this]<typename T>(const T& formattable_datagram)
                                      { return formattable_datagram.serialize(this->_global_tags); },
                                      std::move(datagram)));
}

auto Client::make_local_client(Tags global_tags, uint16_t port) -> Client
{
    return Client {UDPClient::make_local_udp_client(port), std::move(global_tags)};
}

}  // namespace bark
