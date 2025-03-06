#pragma once

#include <cstdio>
#include <source_location>
#include <system_error>
#include <utility>

#include <asio/registered_buffer.hpp>
#include <fmt/base.h>
#include <fmt/std.h>

#include "bark/datagram.hpp"
#include "bark/tags.hpp"

namespace bark::transports
{

template<typename SocketT>
auto sync_send(SocketT& socket, const Tags& global_tags, Datagram&& datagram) -> bool
{
    auto serialized = std::visit([&global_tags](const auto& serializable_datagram)
                                 { return serializable_datagram.serialize(global_tags); },
                                 std::move(datagram));

    auto error = std::error_code {};
    auto bytes_sent = socket.send(asio::buffer(serialized), 0, error);
    if (error) [[unlikely]] {
        fmt::println(stderr, "Failed at sending {}. {}", error.message(), std::source_location::current());
    }
    return bytes_sent == serialized.size();
}

}  // namespace bark::transports
