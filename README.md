# twig-cpp-datadog-client

A modern, low latency datadog stats library for c++

## Getting Started

```cpp :file=./example/getting_started.cpp:line_start=2:line_end=-2
#include "twig/datadog/datadog-client.hpp"

auto main() -> int
{
    using namespace twig::datadog;

    // make a none-async client which sends to the local DogStatsD agent
    auto client = Client::make_local_client();

    // Sending a gauge datagram
    client.send(Gauge("gauge.name", 42.0));

    // Sending a histogram datagram
    client.send(Histogram("histogram.name", 99).with(Tags::from_list({"tag1:hello", "tag2:world"})));

    // Sending a count datagram
    client.send(Count("count.name", 22).with(SampleRate {0.5}));

    // Sending an event datagram
    client.send(Event("event.title", "event.value").with(Priority::NORMAL).with(AlertType::INFO));

    return 0;
}
```

We provide a number of Client implementations for different use cases:

| Client        | Queue type                          | Number of active threads     | Notes                                          |
| ------------- | ----------------------------------- | ---------------------------- | ---------------------------------------------- |
| `Client`      | None                                | The publishing thread only   | Sends metrics on the publishing thread         |
| `SPSCClient`  | Single Producer Single Consumer     | The publishing thread + 1    | Fixed queue size. Drops new messages when full |
| `MPMCClient`  | Multiple Producer Multiple Consumer | The publishing thread + 1    | Fixed queue size. Drops new messages when full |
| `AsioClient`  | `asio::io_context`                  | The publishing thread + `N`  | `N` is supplied when creating the client       |
| `NoOpClient`  | None                                | (The publishing thread)/None | Useful for testing                             |

All of these clients implement the `IDatadogClient` interface. You are not required to use it, but it is there to allow mocking and ease of use for those that need it.

## Requirements
We use modern C++ features and as such you are required to use a newish compiler. We test the library in CI with GCC-13 and Clang-18. We are open for PRs adding Mac and Windows CI checks as well.

## Benchmarks

We benchmark both the metric creation and serialization time as well as the latency of the main thread to send off a datagram.

See [benchmarks/source](./benchmarks/source) for the benchmark code.

#### Clang-18 Results
```
Run ./build/benchmarks/twig-cpp-datadog-client_benchmarks
2024-09-20T11:13:51+00:00
Running ./build/benchmarks/twig-cpp-datadog-client_benchmarks
Run on (4 X 3244.17 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x2)
  L1 Instruction 32 KiB (x2)
  L2 Unified 512 KiB (x2)
  L3 Unified 32768 KiB (x1)
Load Average: 3.02, 2.22, 1.06
------------------------------------------------------------------------------------------------------
Benchmark                                                            Time             CPU   Iterations
------------------------------------------------------------------------------------------------------
benchmark_client_send_metric<Client>                              7843 ns         7843 ns        90051
benchmark_client_send_metric<AsioClient>                           243 ns          243 ns      2829947
benchmark_client_send_metric<SPSCClient>/iterations:1000000       91.2 ns         91.2 ns      1000000
benchmark_client_send_metric<MPMCClient>/iterations:1000000       84.4 ns         84.4 ns      1000000
benchmark_client_send_event<Client>                               7780 ns         7779 ns        90227
benchmark_client_send_event<AsioClient>                            268 ns          268 ns      2633388
benchmark_client_send_event<SPSCClient>/iterations:1000000        99.1 ns         99.1 ns      1000000
benchmark_client_send_event<MPMCClient>/iterations:1000000        93.4 ns         93.4 ns      1000000
benchmark_event_serialize                                          209 ns          209 ns      3347061
benchmark_metric_serialize<Count>                                  212 ns          212 ns      3312071
benchmark_metric_serialize<Gauge>                                  198 ns          198 ns      3541317
benchmark_metric_serialize<Histogram>                              285 ns          285 ns      2470524
benchmark_udp_client_send_metric                                  7343 ns         7342 ns        95563
```

#### GCC-13 Results

```
Run ./build/benchmarks/twig-cpp-datadog-client_benchmarks
2024-09-20T11:19:05+00:00
Running ./build/benchmarks/twig-cpp-datadog-client_benchmarks
Run on (4 X 3290.28 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x2)
  L1 Instruction 32 KiB (x2)
  L2 Unified 512 KiB (x2)
  L3 Unified 32768 KiB (x1)
Load Average: 1.76, 0.67, 0.31
------------------------------------------------------------------------------------------------------
Benchmark                                                            Time             CPU   Iterations
------------------------------------------------------------------------------------------------------
benchmark_client_send_metric<Client>                              7895 ns         7894 ns        89456
benchmark_client_send_metric<AsioClient>                           244 ns          244 ns      2886157
benchmark_client_send_metric<SPSCClient>/iterations:1000000       85.6 ns         85.6 ns      1000000
benchmark_client_send_metric<MPMCClient>/iterations:1000000       79.7 ns         79.6 ns      1000000
benchmark_client_send_event<Client>                               7740 ns         7739 ns        90413
benchmark_client_send_event<AsioClient>                            253 ns          253 ns      2771835
benchmark_client_send_event<SPSCClient>/iterations:1000000        94.1 ns         94.0 ns      1000000
benchmark_client_send_event<MPMCClient>/iterations:1000000        87.0 ns         87.0 ns      1000000
benchmark_event_serialize                                          226 ns          226 ns      3110870
benchmark_metric_serialize<Count>                                  230 ns          230 ns      3036371
benchmark_metric_serialize<Gauge>                                  201 ns          201 ns      3494570
benchmark_metric_serialize<Histogram>                              297 ns          297 ns      2359632
benchmark_udp_client_send_metric                                  7425 ns         7424 ns        94534
```



## Building and installing

Create a copy of `CMakeUserPresets.template.json` called `CMakeUserPreset.json`. Fix the `VCPKG_ROOT` environment variable in the new preset file, to point to somewhere on your system where you have vcpkg installed. Now you can run with presets `dev` etc.

You might want to remove the `clang-tidy` preset inherit from `dev-common` while developing to **greatly** speed up compilation time.

