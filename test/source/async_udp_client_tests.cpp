#include <barrier>
#include <cstdint>
#include <string>
#include <string_view>
#include <utility>

#include "twig/datadog/async_udp_client.hpp"

#include <doctest/doctest.h>

#include "./details/raii_async_context.hpp"

namespace twig::datadog
{

TEST_SUITE("AsyncUDPClient")
{
    TEST_CASE("when sending a message over UDP, the server should receive it")
    {
        auto barrier = std::barrier<>(2);
        auto port = uint16_t {18127};
        constexpr static std::string sent_msg = "hello:42|g";
        auto server = twig::datadog::make_local_udp_server(  //
            port,
            [&barrier](std::string_view recv_msg)
            {
                CHECK_EQ(sent_msg, recv_msg);
                barrier.arrive_and_drop();
            });

        auto client = AsyncUDPClient::make_local_udp_client(1, port);
        client.send(sent_msg);

        barrier.arrive_and_wait();
    }

    TEST_CASE("when multiple messages over UDP, the server should receive them")
    {
        auto barrier = std::barrier<>(5);
        auto port = uint16_t {18127};
        constexpr static std::string sent_msg = "hello:42|g";
        auto server = twig::datadog::make_local_udp_server(  //
            port,
            [&barrier](std::string_view recv_msg)
            {
                CHECK_EQ(sent_msg, recv_msg);
                barrier.arrive_and_drop();
            });

        auto client = AsyncUDPClient::make_local_udp_client(1, port);
        client.send(sent_msg);
        client.send(sent_msg);
        client.send(sent_msg);
        client.send(sent_msg);

        barrier.arrive_and_wait();
    }

    TEST_CASE("when moved the client should still work")
    {
        auto barrier = std::barrier<>(3);
        auto port = uint16_t {18127};
        constexpr static std::string sent_msg = "hello:42|g";
        auto server = twig::datadog::make_local_udp_server(  //
            port,
            [&barrier](std::string_view recv_msg)
            {
                CHECK_EQ(sent_msg, recv_msg);
                barrier.arrive_and_drop();
            });

        auto client = AsyncUDPClient::make_local_udp_client(1, port);
        client.send(sent_msg);
        auto moved_client = std::move(client);
        moved_client.send(sent_msg);

        barrier.arrive_and_wait();
    }
}

}  // namespace twig::datadog
