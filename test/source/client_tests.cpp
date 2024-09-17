#include <barrier>
#include <string_view>
#include <utility>

#include "twig/datadog/client.hpp"

#include <doctest/doctest.h>

#include "./details/raii_async_context.hpp"
#include "twig/datadog/tags.hpp"
#include "twig/datadog/udp_client.hpp"

namespace twig::datadog
{

TEST_SUITE("Client")
{
    TEST_CASE("Correctly sends gauge metrics")
    {
        auto received = false;
        {
            auto barrier = std::barrier<>(2);
            auto port = uint16_t {18127};  // NOLINT
            constexpr std::string_view expected_msg = "gauge.name:43|g|#tag1:hello,tag2:world";
            auto server =
                twig::datadog::make_local_udp_server(port,
                                                     [&received, &expected_msg, &barrier](std::string_view recv_msg)
                                                     {
                                                         received = recv_msg == expected_msg;
                                                         barrier.arrive_and_drop();
                                                     });

            auto client = Client(UDPClient::make_local_udp_client(port));
            client.send_async(Gauge("gauge.name", 43.0).with(Tags::from_tags({"tag1:hello", "tag2:world"})));
            barrier.arrive_and_wait();
        }
        CHECK(received);
    }

    TEST_CASE("Can correctly send gauge metrics after being moved")
    {
        auto received = false;
        {
            auto barrier = std::barrier<>(2);
            auto port = uint16_t {18127};  // NOLINT
            constexpr std::string_view expected_msg = "gauge.name:43|g|#tag1:hello,tag2:world";
            auto server =
                twig::datadog::make_local_udp_server(port,
                                                     [&received, &expected_msg, &barrier](std::string_view recv_msg)
                                                     {
                                                         received = recv_msg == expected_msg;
                                                         barrier.arrive_and_drop();
                                                     });

            auto client = Client(UDPClient::make_local_udp_client(port));
            auto moved = std::move(client);
            moved.send_async(Gauge("gauge.name", 43.0).with(Tags::from_tags({"tag1:hello", "tag2:world"})));
            barrier.arrive_and_wait();
        }
        CHECK(received);
    }
}

}  // namespace twig::datadog
