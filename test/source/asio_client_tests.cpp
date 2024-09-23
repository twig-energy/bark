#include <barrier>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include <thread>
#include <utility>

#include "twig/datadog/asio_client.hpp"

#include <doctest/doctest.h>

#include "./details/raii_async_context.hpp"
#include "twig/datadog/number_of_io_threads.hpp"
#include "twig/datadog/tags.hpp"

namespace twig::datadog
{

TEST_SUITE("AsioClient")
{
    TEST_CASE("Correctly sends gauge metrics")
    {
        constexpr static std::string_view expected_msg = "gauge.name:43|g|#tag1:hello,tag2:world";
        auto barrier = std::barrier<>(2);
        auto port = uint16_t {18127};
        auto server = twig::datadog::make_local_udp_server(  //
            port,
            [&barrier](std::string_view recv_msg)
            {
                CHECK_EQ(recv_msg, expected_msg);
                barrier.arrive_and_drop();
            });

        auto client = AsioClient::make_local_client(NumberOfIOThreads {1}, no_tags, port);
        client.send(Gauge("gauge.name", 43.0).with(Tags::from_list({"tag1:hello", "tag2:world"})));
        barrier.arrive_and_wait();
    }

    TEST_CASE("Can correctly send gauge metrics after being moved")
    {
        constexpr static std::string_view expected_msg = "gauge.name:43|g|#tag1:hello,tag2:world";
        auto barrier = std::barrier<>(2);
        auto port = uint16_t {18127};
        auto server = twig::datadog::make_local_udp_server(  //
            port,
            [&barrier](std::string_view recv_msg)
            {
                CHECK_EQ(recv_msg, expected_msg);
                barrier.arrive_and_drop();
            });

        auto client = AsioClient::make_local_client(NumberOfIOThreads {1}, no_tags, port);
        auto moved = std::move(client);
        moved.send(Gauge("gauge.name", 43.0).with(Tags::from_list({"tag1:hello", "tag2:world"})));
        barrier.arrive_and_wait();
    }

    TEST_CASE("Correctly sends gauge metrics with global tags added")
    {
        constexpr static std::string_view expected_msg = "gauge.name:43|g|#tag1:hello,tag2:world,language:cpp,foo:bar";
        auto barrier = std::barrier<>(2);
        auto port = uint16_t {18127};
        auto server = twig::datadog::make_local_udp_server(  //
            port,
            [&barrier](std::string_view recv_msg)
            {
                CHECK_EQ(recv_msg, expected_msg);
                barrier.arrive_and_drop();
            });

        auto client =
            AsioClient::make_local_client(NumberOfIOThreads {1}, Tags::from_list({"language:cpp", "foo:bar"}), port);
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

        auto client = AsioClient::make_local_client(NumberOfIOThreads {2}, Tags::from_list({"env:test"}), port);

        for (auto i = 0; i < 1'000; i++) {   // Simulate 1000 ticks
            for (auto j = 0; j < 50; j++) {  // That each sends 50 metrics
                client.send(Gauge("gauge.name", 43.0).with(Tags::from_list({"tag1:hello", "tag2:world"})));
            }

            // Small wait
            std::this_thread::sleep_for(std::chrono::microseconds(500));
        }
    }

    TEST_CASE("queue gets emptied after client has been destroyed")
    {
        auto port = uint16_t {18127};
        auto server_barrier = std::barrier<>(2);
        constexpr std::string_view expected_msg = "gauge.name:43|g|#tag1:hello,tag2:world";

        auto sender_barrier = std::barrier<>(2);
        auto received = std::size_t {0};
        auto server = twig::datadog::make_local_udp_server(
            port,
            [&received, &expected_msg, &server_barrier, &sender_barrier](std::string_view recv_msg)
            {
                if (received == 0) {
                    sender_barrier.arrive_and_drop();
                }

                received += static_cast<std::size_t>(recv_msg == expected_msg);

                if (received == 2) {
                    server_barrier.arrive_and_drop();
                }
            });

        {
            auto client = AsioClient::make_local_client(NumberOfIOThreads {1}, no_tags, port);

            // send the first so we can wait for the server to receive
            client.send(Gauge("gauge.name", 43.0).with(Tags::from_list({"tag1:hello", "tag2:world"})));

            // we now know that the worker thread has started
            sender_barrier.arrive_and_wait();

            // send the second and immediately destroy the client
            client.send(Gauge("gauge.name", 43.0).with(Tags::from_list({"tag1:hello", "tag2:world"})));
        }

        // wait for messages to arrive at server
        server_barrier.arrive_and_wait();
        CHECK_EQ(received, 2);
    }
}

}  // namespace twig::datadog
