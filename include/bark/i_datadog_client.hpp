#pragma once

#include "bark/datagram.hpp"

namespace bark
{

struct IDatadogClient
{
    constexpr IDatadogClient() = default;
    constexpr IDatadogClient(const IDatadogClient&) = default;
    constexpr IDatadogClient(IDatadogClient&&) noexcept = default;
    constexpr auto operator=(const IDatadogClient&) -> IDatadogClient& = default;
    constexpr auto operator=(IDatadogClient&&) noexcept -> IDatadogClient& = default;
    constexpr virtual ~IDatadogClient() = default;

    virtual auto send(const Datagram& datagram) -> void = 0;
    virtual auto send(Datagram&& datagram) -> void = 0;
};

}  // namespace bark
