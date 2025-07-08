#include "bark/bark.hpp"

auto main() -> int
{
    using namespace bark;

    // make a none-async client which sends to the local DogStatsD agent
    auto client = Client::make_local_client();

    // Sending a gauge datagram
    client.send(Gauge("gauge.name", 42.0));

    // Sending a histogram datagram
    client.send(Histogram("histogram.name", 99).with(Tags::from_list({"tag1:hello", "tag2:world"})));

    // Sending a count datagram
    client.send(Count("count.name", 22).with(SampleRate {0.5}));

    // Sending an event datagram
    client.send(Event("event.title", "event.value").with(Priority::Normal).with(AlertType::Info));

    return 0;
}
