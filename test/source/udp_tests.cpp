#include <barrier>
#include <cstdint>
#include <string_view>

#include <doctest/doctest.h>

#include "./raii_async_context.hpp"
#include "twig/datadog/udp_client.hpp"

namespace twig::datadog
{

TEST_SUITE("UDPClient")
{
    TEST_CASE("when sending a message over UDP, the server should receive it")
    {
        auto received = false;
        {
            auto barrier = std::barrier<>(2);
            auto port = uint16_t {18127};  // NOLINT
            constexpr std::string_view sent_msg = "hello:42|g";
            auto server =
                twig::datadog::make_local_udp_server(port,
                                                     [&received, &sent_msg, &barrier](std::string_view recv_msg)
                                                     {
                                                         received = recv_msg == sent_msg;
                                                         barrier.arrive_and_wait();
                                                     });

            auto io_context = asio::io_context();
            auto client = UDPClient(io_context, "localhost", port);
            CHECK(client.send(sent_msg));

            barrier.arrive_and_wait();
        }
        CHECK(received);
    }

    TEST_CASE("when asynchronously sending a message over UDP, the server should receive it")
    {
        auto received = 0;
        {
            auto barrier = std::barrier<>(5);
            auto port = uint16_t {18127};  // NOLINT
            constexpr std::string_view sent_msg = "hello:42|g";
            auto server =
                twig::datadog::make_local_udp_server(port,
                                                     [&received, &sent_msg, &barrier](std::string_view recv_msg)
                                                     {
                                                         if (recv_msg == sent_msg) {
                                                             received++;
                                                         }

                                                         // Drop to not block the server.
                                                         barrier.arrive_and_drop();
                                                     });

            auto io_context = asio::io_context();
            auto client = UDPClient(io_context, "localhost", port);
            client.send_async(sent_msg);
            client.send_async(sent_msg);
            client.send_async(sent_msg);
            client.send_async(sent_msg);

            barrier.arrive_and_wait();
        }
        CHECK(received == 4);
    }
}

}  // namespace twig::datadog
