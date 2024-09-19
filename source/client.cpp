#include <utility>
#include <variant>

#include "twig/datadog/client.hpp"

#include "twig/datadog/datagram.hpp"
#include "twig/datadog/tags.hpp"
#include "twig/datadog/udp_client.hpp"

namespace twig::datadog
{

Client::Client(UDPClient&& udp_client)
    : _udp_client(std::move(udp_client))
{
}

Client::Client(UDPClient&& udp_client, Tags global_tags)
    : _udp_client(std::move(udp_client))
    , _global_tags(std::move(global_tags))
{
}

auto Client::send_async(const Datagram& datagram) -> void
{
    this->_udp_client.send_async(std::visit([this](const auto& serializable_datagram)
                                            { return serializable_datagram.serialize(this->_global_tags); },
                                            datagram));
}

auto Client::send_async(Datagram&& datagram) -> void
{
    this->_udp_client.send_async(std::visit([this]<typename T>(const T& formattable_datagram)
                                            { return formattable_datagram.serialize(this->_global_tags); },
                                            std::move(datagram)));
}

}  // namespace twig::datadog
