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
auto async_send(asio::io_context& io_context,
                const std::unique_ptr<SocketT>& socket,
                Datagram&& datagram,
                const std::unique_ptr<Tags>& global_tags) -> void
{
    asio::post(
        io_context,
        [socket_ptr = socket.get(), global_tags_ptr = global_tags.get(), datagram = std::move(datagram)]() mutable
        {
            auto message =
                std::make_unique<std::string>(std::visit([global_tags_ptr](const auto& serializable_datagram)
                                                         { return serializable_datagram.serialize(*global_tags_ptr); },
                                                         datagram));
            auto buffer = asio::buffer(*message);

            socket_ptr->async_send(
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
