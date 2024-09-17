#include "twig/datadog/client.hpp"
#include "twig/datadog/count.hpp"
#include "twig/datadog/gauge.hpp"
#include "twig/datadog/histogram.hpp"
#include "twig/datadog/tags.hpp"
#include "twig/datadog/udp_client.hpp"

auto main() -> int
{
    using namespace twig::datadog;  // NOLINT
    // make an none-async client which sends to the local DogStatsD agent
    auto client = Client(UDPClient::make_local_udp_client());

    // Sending a gauge metric
    client.send_async(Gauge("gauge.name", 42.0));

    // Sending a histogram metric
    client.send_async(Histogram("histogram.name", 99).with(Tags::from_tags({"tag1:hello", "tag2:world"})));

    // Sending a count metric
    client.send_async(Count("count.name", 22).with(SampleRate {0.5}));

    return 0;
}
