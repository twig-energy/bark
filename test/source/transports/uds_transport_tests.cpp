#include <barrier>
#include <filesystem>
#include <string_view>
#include <utility>

#include "bark/transports/uds_transport.hpp"

#include <doctest/doctest.h>

#include "../details/raii_async_context.hpp"

namespace bark
{

TEST_SUITE("UDSTransport")
{
    TEST_CASE("when sending a message over UDS, the server should receive it")
    {
        auto barrier = std::barrier<>(2);
        auto socket_path = std::filesystem::path {"bark_test.sock"};
        auto metric = Gauge("hello", 42);
        constexpr std::string_view sent_msg = "hello:42|g";
        auto server = bark::make_uds_server(  //
            socket_path,
            [&sent_msg, &barrier](std::string_view recv_msg)
            {
                CHECK_EQ(sent_msg, recv_msg);
                barrier.arrive_and_drop();
            });

        auto client = transports::UDSTransport {socket_path};
        CHECK(client.send(std::move(metric)));

        barrier.arrive_and_wait();
    }

    TEST_CASE("when multiple messages over UDS, the server should receive them")
    {
        auto barrier = std::barrier<>(5);
        auto socket_path = std::filesystem::path {"bark_test.sock"};
        auto metric = Gauge("hello", 42);
        constexpr std::string_view sent_msg = "hello:42|g";
        auto server = bark::make_uds_server(  //
            socket_path,
            [&sent_msg, &barrier](std::string_view recv_msg)
            {
                CHECK_EQ(sent_msg, recv_msg);
                barrier.arrive_and_drop();
            });

        auto client = transports::UDSTransport {socket_path};
        CHECK(client.send(metric));
        CHECK(client.send(metric));
        CHECK(client.send(metric));
        CHECK(client.send(std::move(metric)));

        barrier.arrive_and_wait();
    }

    TEST_CASE("when moved the client should still work")
    {
        auto barrier = std::barrier<>(3);
        auto socket_path = std::filesystem::path {"bark_test.sock"};
        auto metric = Gauge("hello", 42);
        constexpr std::string_view sent_msg = "hello:42|g";
        auto server = bark::make_uds_server(  //
            socket_path,
            [&sent_msg, &barrier](std::string_view recv_msg)
            {
                CHECK_EQ(sent_msg, recv_msg);
                barrier.arrive_and_drop();
            });

        auto client = transports::UDSTransport {socket_path};
        CHECK(client.send(metric));
        auto moved_client = std::move(client);
        CHECK(moved_client.send(std::move(metric)));

        barrier.arrive_and_wait();
    }
}

}  // namespace bark
