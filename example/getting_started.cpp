#include "twig/datadog/datadog-client.hpp"

auto main() -> int
{
    using namespace twig::datadog;  // NOLINT for brevity

    // make an none-async client which sends to the local DogStatsD agent
    auto client = Client(UDPClient::make_local_udp_client());

    // Sending a gauge datagram
    client.send_async(Gauge("gauge.name", 42.0));

    // Sending a histogram datagram
    client.send_async(Histogram("histogram.name", 99).with(Tags::from_tags({"tag1:hello", "tag2:world"})));

    // Sending a count datagram
    client.send_async(Count("count.name", 22).with(SampleRate {0.5}));

    // Sending an event datagram
    client.send_async(Event("event.title", "event.value").with(Priority::NORMAL).with(AlertType::INFO));

    return 0;
}
