# twig-cpp-datadog-client

A modern, low latency datadog stats library for c++

## Getting Started
```cpp :file=./example/getting_started.cpp
#include "twig/datadog/datadog-client.hpp"

auto main() -> int
{
    using namespace twig::datadog;

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
```

We provide a number of Client implementations for different use cases:
- `Client` which eagerly sends metrics
- `SPSCClient` which adds metrics to a Single Producer Single Consumer queue.
- `MPMCClient` which adds metrics to a Multiple Producer Multiple Consumer queue.
- `NoOpClient` which ignores the metrics (useful for testing)


## Building and installing

Create a copy of `CMakeUserPresets.template.json` called `CMakeUserPreset.json`. Fix the `VCPKG_ROOT` environment variable in the new preset file, to point to somewhere on your system where you have vcpkg installed. Now you can run with presets `dev` etc.

You might want too remove the `clang-tidy` preset inherit from `dev-common` while developing to **greatly** speed up compilation time.

