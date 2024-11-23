#include <barrier>
#include <cstdint>
#include <string_view>
#include <utility>

#include "bark/transports/udp_transport.hpp"

#include <doctest/doctest.h>

#include "../details/raii_async_context.hpp"

namespace bark
{

TEST_SUITE("UDPTransport")
{
    TEST_CASE("when sending a message over UDP, the server should receive it")
    {
        auto barrier = std::barrier<>(2);
        auto port = uint16_t {18127};
        constexpr std::string_view sent_msg = "hello:42|g";
        auto server = bark::make_local_udp_server(  //
            port,
            [&sent_msg, &barrier](std::string_view recv_msg)
            {
                CHECK_EQ(sent_msg, recv_msg);
                barrier.arrive_and_drop();
            });

        auto client = transports::UDPTransport::make_local_udp_transport(port);
        CHECK(client.send(sent_msg));

        barrier.arrive_and_wait();
    }

    TEST_CASE("when multiple messages over UDP, the server should receive them")
    {
        auto barrier = std::barrier<>(5);
        auto port = uint16_t {18127};
        constexpr std::string_view sent_msg = "hello:42|g";
        auto server = bark::make_local_udp_server(  //
            port,
            [&sent_msg, &barrier](std::string_view recv_msg)
            {
                CHECK_EQ(sent_msg, recv_msg);
                barrier.arrive_and_drop();
            });

        auto client = transports::UDPTransport::make_local_udp_transport(port);
        CHECK(client.send(sent_msg));
        CHECK(client.send(sent_msg));
        CHECK(client.send(sent_msg));
        CHECK(client.send(sent_msg));

        barrier.arrive_and_wait();
    }

    TEST_CASE("when moved the client should still work")
    {
        auto barrier = std::barrier<>(3);
        auto port = uint16_t {18127};
        constexpr std::string_view sent_msg = "hello:42|g";
        auto server = bark::make_local_udp_server(  //
            port,
            [&sent_msg, &barrier](std::string_view recv_msg)
            {
                CHECK_EQ(sent_msg, recv_msg);
                barrier.arrive_and_drop();
            });

        auto client = transports::UDPTransport::make_local_udp_transport(port);
        CHECK(client.send(sent_msg));
        auto moved_client = std::move(client);
        CHECK(moved_client.send(sent_msg));

        barrier.arrive_and_wait();
    }
}

}  // namespace bark
