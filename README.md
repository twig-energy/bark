# twig-cpp-datadog-client

A modern, low latency datadog stats library for c++

## Getting Started

```cpp :file=./example/getting_started.cpp:line_start=2:line_end=-2
#include "twig/datadog/datadog-client.hpp"

auto main() -> int
{
    using namespace twig::datadog;

    // make a none-async client which sends to the local DogStatsD agent
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

All of these clients implement the `IDatadogClient` interface. You are not required to use it, but it is there to allow mocking and ease of use for those that need it.

## Requirements
We use modern C++ features and as such you are required to use a newish compiler. We test the library in CI with GCC-13 and Clang-18. We are open for PRs adding Mac and Windows CI checks as well.

## Benchmarks

We benchmark both the metric creation and serialization time as well as the latency of the main thread to send off a datagram.

See [benchmarks/source](./benchmarks/source) for the benchmark code.

#### Clang-18 Results
```
Run ./build/benchmarks/twig-cpp-datadog-client_benchmarks
2024-09-17T12:51:17+00:00
Running ./build/benchmarks/twig-cpp-datadog-client_benchmarks
Run on (4 X 3032.16 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x2)
  L1 Instruction 32 KiB (x2)
  L2 Unified 512 KiB (x2)
  L3 Unified 32768 KiB (x1)
Load Average: 3.00, 1.67, 0.69
------------------------------------------------------------------------------------------------------------
Benchmark                                                                  Time             CPU   Iterations
------------------------------------------------------------------------------------------------------------
benchmark_client_send_metric_async<Client>                              8257 ns         8251 ns        86938
benchmark_client_send_metric_async<SPSCClient>/iterations:1000000       80.1 ns         80.0 ns      1000000
benchmark_client_send_metric_async<MPMCClient>/iterations:1000000       58.5 ns         58.5 ns      1000000
benchmark_client_send_event_async<Client>                               7834 ns         7832 ns        89432
benchmark_client_send_event_async<SPSCClient>/iterations:1000000        59.2 ns         59.2 ns      1000000
benchmark_client_send_event_async<MPMCClient>/iterations:1000000        66.6 ns         66.6 ns      1000000
benchmark_event_serialize                                                240 ns          240 ns      2917293
benchmark_metric_serialize<Count>                                        225 ns          225 ns      3130795
benchmark_metric_serialize<Gauge>                                        212 ns          212 ns      3297873
benchmark_metric_serialize<Histogram>                                    313 ns          313 ns      2230205
```

#### GCC-13 Results

```
Run ./build/benchmarks/twig-cpp-datadog-client_benchmarks
2024-09-17T12:56:26+00:00
Running ./build/benchmarks/twig-cpp-datadog-client_benchmarks
Run on (4 X 3246.74 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x2)
  L1 Instruction 32 KiB (x2)
  L2 Unified 512 KiB (x2)
  L3 Unified 32768 KiB (x1)
Load Average: 1.57, 0.48, 0.19
------------------------------------------------------------------------------------------------------------
Benchmark                                                                  Time             CPU   Iterations
------------------------------------------------------------------------------------------------------------
benchmark_client_send_metric_async<Client>                              8035 ns         8032 ns        87194
benchmark_client_send_metric_async<SPSCClient>/iterations:1000000       88.5 ns         88.5 ns      1000000
benchmark_client_send_metric_async<MPMCClient>/iterations:1000000       60.9 ns         60.9 ns      1000000
benchmark_client_send_event_async<Client>                               8006 ns         8002 ns        88864
benchmark_client_send_event_async<SPSCClient>/iterations:1000000        66.9 ns         66.9 ns      1000000
benchmark_client_send_event_async<MPMCClient>/iterations:1000000        65.9 ns         65.9 ns      1000000
benchmark_event_serialize                                                243 ns          243 ns      2882897
benchmark_metric_serialize<Count>                                        251 ns          251 ns      2789760
benchmark_metric_serialize<Gauge>                                        229 ns          229 ns      3074499
benchmark_metric_serialize<Histogram>                                    314 ns          314 ns      2230463
```



## Building and installing

Create a copy of `CMakeUserPresets.template.json` called `CMakeUserPreset.json`. Fix the `VCPKG_ROOT` environment variable in the new preset file, to point to somewhere on your system where you have vcpkg installed. Now you can run with presets `dev` etc.

You might want to remove the `clang-tidy` preset inherit from `dev-common` while developing to **greatly** speed up compilation time.

