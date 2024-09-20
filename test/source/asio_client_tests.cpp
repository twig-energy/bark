#include <barrier>
#include <chrono>
#include <cstdint>
#include <string_view>
#include <thread>
#include <utility>

#include "twig/datadog/asio_client.hpp"

#include <doctest/doctest.h>

#include "./details/raii_async_context.hpp"
#include "twig/datadog/tags.hpp"

namespace twig::datadog
{

TEST_SUITE("AsioClient")
{
    TEST_CASE("Correctly sends gauge metrics")
    {
        auto received = false;
        {
            auto barrier = std::barrier<>(2);
            auto port = uint16_t {18127};
            constexpr std::string_view expected_msg = "gauge.name:43|g|#tag1:hello,tag2:world";
            auto server =
                twig::datadog::make_local_udp_server(port,
                                                     [&received, &expected_msg, &barrier](std::string_view recv_msg)
                                                     {
                                                         received = recv_msg == expected_msg;
                                                         barrier.arrive_and_drop();
                                                     });

            auto client = AsioClient::make_local_client(1, no_tags, port);
            client.send(Gauge("gauge.name", 43.0).with(Tags::from_list({"tag1:hello", "tag2:world"})));
            barrier.arrive_and_wait();
        }
        CHECK(received);
    }

    TEST_CASE("Can correctly send gauge metrics after being moved")
    {
        auto received = false;
        {
            auto barrier = std::barrier<>(2);
            auto port = uint16_t {18127};
            constexpr std::string_view expected_msg = "gauge.name:43|g|#tag1:hello,tag2:world";
            auto server =
                twig::datadog::make_local_udp_server(port,
                                                     [&received, &expected_msg, &barrier](std::string_view recv_msg)
                                                     {
                                                         received = recv_msg == expected_msg;
                                                         barrier.arrive_and_drop();
                                                     });

            auto client = AsioClient::make_local_client(1, no_tags, port);
            auto moved = std::move(client);
            moved.send(Gauge("gauge.name", 43.0).with(Tags::from_list({"tag1:hello", "tag2:world"})));
            barrier.arrive_and_wait();
        }
        CHECK(received);
    }

    TEST_CASE("Correctly sends gauge metrics with global tags added")
    {
        auto barrier = std::barrier<>(2);
        auto port = uint16_t {18127};
        constexpr std::string_view expected_msg = "gauge.name:43|g|#tag1:hello,tag2:world,language:cpp,foo:bar";
        auto server = twig::datadog::make_local_udp_server(port,
                                                           [&expected_msg, &barrier](std::string_view recv_msg)
                                                           {
                                                               CHECK_EQ(recv_msg, expected_msg);
                                                               barrier.arrive_and_drop();
                                                           });

        auto client = AsioClient::make_local_client(1, Tags::from_list({"language:cpp", "foo:bar"}), port);
        client.send(Gauge("gauge.name", 43.0).with(Tags::from_list({"tag1:hello", "tag2:world"})));
        barrier.arrive_and_wait();
    }

    TEST_CASE("Can send many metrics without leaking")
    {
        auto port = uint16_t {18127};
        auto server = twig::datadog::make_local_udp_server(port,
                                                           []([[maybe_unused]]
                                                              std::string_view recv_msg)
                                                           {
                                                               // Just consume message.
                                                           });

        auto client = AsioClient::make_local_client(2, Tags::from_list({"env:test"}), port);

        for (auto i = 0; i < 1'000; i++) {   // Simulate 1000 ticks
            for (auto j = 0; j < 50; j++) {  // That each sends 50 metrics
                client.send(Gauge("gauge.name", 43.0).with(Tags::from_list({"tag1:hello", "tag2:world"})));
            }

            // Small wait
            std::this_thread::sleep_for(std::chrono::microseconds(500));
        }
    }
}

}  // namespace twig::datadog
