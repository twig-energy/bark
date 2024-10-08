#include <barrier>
#include <cstdint>
#include <string_view>
#include <utility>

#include "bark/client.hpp"

#include <doctest/doctest.h>

#include "./details/raii_async_context.hpp"
#include "bark/tags.hpp"

namespace bark
{

TEST_SUITE("Client")
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

            auto client = Client::make_local_client(no_tags, port);
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

            auto client = Client::make_local_client(no_tags, port);
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
        auto server = bark::make_local_udp_server(port,
                                                  [&expected_msg, &barrier](std::string_view recv_msg)
                                                  {
                                                      CHECK_EQ(recv_msg, expected_msg);
                                                      barrier.arrive_and_drop();
                                                  });

        auto client = Client::make_local_client(Tags::from_list({"language:cpp", "foo:bar"}), port);
        client.send(Gauge("gauge.name", 43.0).with(Tags::from_list({"tag1:hello", "tag2:world"})));
        barrier.arrive_and_wait();
    }
}

}  // namespace bark
