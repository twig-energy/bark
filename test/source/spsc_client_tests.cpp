#include <barrier>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include <utility>

#include "bark/spsc_client.hpp"

#include <doctest/doctest.h>

#include "./details/raii_async_context.hpp"
#include "bark/tags.hpp"

namespace bark
{

TEST_SUITE("SPSCClient")
{
    TEST_CASE("Correctly sends gauge metrics")
    {
        auto received = false;
        {
            auto barrier = std::barrier<>(2);
            auto port = uint16_t {18127};
            constexpr std::string_view expected_msg = "gauge.name:43|g|#tag1:hello,tag2:world";
            auto server = bark::make_local_udp_server(port,
                                                      [&received, &expected_msg, &barrier](std::string_view recv_msg)
                                                      {
                                                          received = recv_msg == expected_msg;
                                                          barrier.arrive_and_drop();
                                                      });

            auto queue_size = size_t {1};
            auto client = SPSCClient::make_local_udp_client(queue_size, no_tags, port);
            client.send(Gauge("gauge.name", 43.0).with(Tags::from_list({"tag1:hello", "tag2:world"})));
            barrier.arrive_and_wait();
        }
        CHECK(received);
    }

    TEST_CASE("will not block if queue is full")
    {
        auto received = false;
        {
            auto barrier = std::barrier<>(2);
            auto port = uint16_t {18127};
            constexpr std::string_view expected_msg = "gauge.name:43|g|#tag1:hello,tag2:world";
            auto server = bark::make_local_udp_server(port,
                                                      [&received, &expected_msg, &barrier](std::string_view recv_msg)
                                                      {
                                                          barrier.arrive_and_drop();
                                                          received = recv_msg == expected_msg;
                                                      });

            auto queue_size = size_t {1};
            auto client = SPSCClient::make_local_udp_client(queue_size, no_tags, port);
            client.send(Gauge("gauge.name", 43.0).with(Tags::from_list({"tag1:hello", "tag2:world"})));
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
            auto server = bark::make_local_udp_server(port,
                                                      [&received, &expected_msg, &barrier](std::string_view recv_msg)
                                                      {
                                                          received = recv_msg == expected_msg;
                                                          barrier.arrive_and_drop();
                                                      });
            auto queue_size = size_t {1};
            auto client = SPSCClient::make_local_udp_client(queue_size, no_tags, port);
            auto moved = std::move(client);
            moved.send(Gauge("gauge.name", 43.0).with(Tags::from_list({"tag1:hello", "tag2:world"})));
            barrier.arrive_and_wait();
        }
        CHECK(received);
    }

    TEST_CASE("queue gets emptied after client has been destroyed")
    {
        auto port = uint16_t {18127};
        auto server_barrier = std::barrier<>(2);
        constexpr std::string_view expected_msg = "gauge.name:43|g|#tag1:hello,tag2:world";

        auto sender_barrier = std::barrier<>(2);
        auto received = std::size_t {0};
        auto server = bark::make_local_udp_server(
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
            auto client = SPSCClient::make_local_udp_client(2, no_tags, port);

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

}  // namespace bark
