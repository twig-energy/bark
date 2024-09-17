#include <utility>
#include <variant>

#include "twig/datadog/client.hpp"

#include "twig/datadog/datagram.hpp"
#include "twig/datadog/udp_client.hpp"

namespace twig::datadog
{

Client::Client(UDPClient&& udp_client)
    : _udp_client(std::move(udp_client))
{
}

auto Client::send_async(const Datagram& datagram) -> void
{
    this->_udp_client.send_async(
        std::visit([](const auto& serializable_datagram) { return serializable_datagram.serialize(); }, datagram));
}

auto Client::send_async(Datagram&& datagram) -> void
{
    this->_udp_client.send_async(std::visit([]<typename T>(const T& formattable_datagram)
                                            { return formattable_datagram.serialize(); },
                                            std::move(datagram)));
}

}  // namespace twig::datadog
