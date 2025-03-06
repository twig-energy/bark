#include <cstdio>
#include <memory>
#include <source_location>
#include <string>
#include <system_error>
#include <utility>
#include <variant>

#include <asio/io_context.hpp>
#include <asio/post.hpp>
#include <asio/registered_buffer.hpp>
#include <fmt/base.h>
#include <fmt/std.h>

#include "bark/asio_io_context_wrapper.hpp"
#include "bark/datagram.hpp"
#include "bark/tags.hpp"

namespace bark::transports
{

template<typename SocketT>
auto async_send(asio::io_context& io_context, SocketT& socket, const Tags& global_tags, Datagram&& datagram) -> void
{
    asio::post(
        io_context,
        [&socket, &global_tags, datagram_ = std::move(datagram)]()
        {
            auto message =
                std::make_unique<std::string>(std::visit([&global_tags](const auto& serializable_datagram)
                                                         { return serializable_datagram.serialize(global_tags); },
                                                         datagram_));
            auto buffer = asio::buffer(*message);

            socket.async_send(
                buffer,
                [msg = std::move(message)](const std::error_code& error, std::size_t)
                {
                    // Move in `message`, to make the callback clean up, such that we are sure we are not using a
                    // cleaned up string.
                    if (error) [[unlikely]] {
                        fmt::println(
                            stderr, "Failed at sending {}. {}", error.message(), std::source_location::current());
                    }
                });
        });
}

}  // namespace bark::transports
