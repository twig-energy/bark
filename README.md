# twig-cpp-datadog-client

A modern, low latency datadog stats library for c++

## Getting Started
```cpp :file=./example/getting_started.cpp
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
    client.send_async(Gauge("gauge.name", 42.0).with_tags(Tags::from_tags({"tag1:hello", "tag2:world"})));

    // Sending a histogram metric
    client.send_async(Histogram("histogram.name", 99).with_tags(Tags::from_tags({"tag1:hello", "tag2:world"})));

    // Sending a count metric
    client.send_async(Count("count.name", 22).with_tags(Tags::from_tags({"tag1:hello", "tag2:world"})));

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

