# bark

A modern, low latency datadog stats library for c++

## Getting Started

```cpp :file=./example/getting_started.cpp:line_start=2:line_end=-2
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
```

We provide a number of Client implementations for different use cases:

| Client       | Queue type                          | Number of active threads     | Notes                                          |
| ------------ | ----------------------------------- | ---------------------------- | ---------------------------------------------- |
| `Client`     | None                                | The publishing thread only   | Sends metrics on the publishing thread         |
| `SPSCClient` | Single Producer Single Consumer     | The publishing thread + 1    | Fixed queue size. Drops new messages when full |
| `MPMCClient` | Multiple Producer Multiple Consumer | The publishing thread + 1    | Fixed queue size. Drops new messages when full |
| `AsioClient` | `asio::io_context`                  | The publishing thread + `N`  | `N` is supplied when creating the client       |
| `NoOpClient` | None                                | (The publishing thread)/None | Useful for testing                             |

All of these clients implement the `IDatadogClient` interface. You are not required to use it, but it is there to allow mocking and ease of use for those that need it.

## Requirements
We use modern C++ features and as such you are required to use a newish compiler. We test the library in CI with GCC-13 and Clang-18. We are open for PRs adding Mac and Windows CI checks as well.

## Benchmarks

We benchmark both the metric creation and serialization time as well as the latency of the main thread to send off a datagram.

See [benchmarks/source](./benchmarks/source) for the benchmark code.

#### Clang-18 Results
```
2024-09-20T12:42:44+00:00
Running ./build/benchmarks/bark_benchmarks
Run on (4 X 3241.38 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x2)
  L1 Instruction 32 KiB (x2)
  L2 Unified 512 KiB (x2)
  L3 Unified 32768 KiB (x1)
Load Average: 3.26, 2.09, 0.92
----------------------------------------------------------------------------------------------------------------------
Benchmark                                                                            Time             CPU   Iterations
----------------------------------------------------------------------------------------------------------------------
benchmark_client_send_metric<Client>/iterations:25000/repeats:32_mean             7735 ns         7734 ns           32
benchmark_client_send_metric<Client>/iterations:25000/repeats:32_median           7719 ns         7719 ns           32
benchmark_client_send_metric<Client>/iterations:25000/repeats:32_stddev           44.1 ns         44.0 ns           32
benchmark_client_send_metric<Client>/iterations:25000/repeats:32_cv               0.57 %          0.57 %            32
benchmark_client_send_event<AsioClient>/iterations:25000/repeats:32_mean           297 ns          296 ns           32
benchmark_client_send_event<AsioClient>/iterations:25000/repeats:32_median         278 ns          278 ns           32
benchmark_client_send_event<AsioClient>/iterations:25000/repeats:32_stddev        57.4 ns         57.5 ns           32
benchmark_client_send_event<AsioClient>/iterations:25000/repeats:32_cv           19.31 %         19.41 %            32
benchmark_client_send_metric<SPSCClient>/iterations:25000/repeats:32_mean         76.6 ns         76.6 ns           32
benchmark_client_send_metric<SPSCClient>/iterations:25000/repeats:32_median       73.7 ns         73.7 ns           32
benchmark_client_send_metric<SPSCClient>/iterations:25000/repeats:32_stddev       12.0 ns         11.9 ns           32
benchmark_client_send_metric<SPSCClient>/iterations:25000/repeats:32_cv          15.60 %         15.60 %            32
benchmark_client_send_metric<MPMCClient>/iterations:25000/repeats:32_mean         74.6 ns         74.5 ns           32
benchmark_client_send_metric<MPMCClient>/iterations:25000/repeats:32_median       73.1 ns         73.1 ns           32
benchmark_client_send_metric<MPMCClient>/iterations:25000/repeats:32_stddev       3.70 ns         3.71 ns           32
benchmark_client_send_metric<MPMCClient>/iterations:25000/repeats:32_cv           4.95 %          4.97 %            32
benchmark_client_send_event<Client>/iterations:25000/repeats:32_mean              7758 ns         7758 ns           32
benchmark_client_send_event<Client>/iterations:25000/repeats:32_median            7755 ns         7755 ns           32
benchmark_client_send_event<Client>/iterations:25000/repeats:32_stddev            15.3 ns         15.3 ns           32
benchmark_client_send_event<Client>/iterations:25000/repeats:32_cv                0.20 %          0.20 %            32
benchmark_client_send_metric<AsioClient>/iterations:25000/repeats:32_mean          146 ns          146 ns           32
benchmark_client_send_metric<AsioClient>/iterations:25000/repeats:32_median        143 ns          142 ns           32
benchmark_client_send_metric<AsioClient>/iterations:25000/repeats:32_stddev       18.6 ns         18.6 ns           32
benchmark_client_send_metric<AsioClient>/iterations:25000/repeats:32_cv          12.73 %         12.77 %            32
benchmark_client_send_event<SPSCClient>/iterations:25000/repeats:32_mean          87.3 ns         87.3 ns           32
benchmark_client_send_event<SPSCClient>/iterations:25000/repeats:32_median        86.4 ns         86.4 ns           32
benchmark_client_send_event<SPSCClient>/iterations:25000/repeats:32_stddev        5.66 ns         5.61 ns           32
benchmark_client_send_event<SPSCClient>/iterations:25000/repeats:32_cv            6.48 %          6.43 %            32
benchmark_client_send_event<MPMCClient>/iterations:25000/repeats:32_mean          96.7 ns         96.6 ns           32
benchmark_client_send_event<MPMCClient>/iterations:25000/repeats:32_median        93.1 ns         93.1 ns           32
benchmark_client_send_event<MPMCClient>/iterations:25000/repeats:32_stddev        12.9 ns         12.8 ns           32
benchmark_client_send_event<MPMCClient>/iterations:25000/repeats:32_cv           13.38 %         13.27 %            32
benchmark_event_serialize/repeats:16_mean                                          228 ns          228 ns           16
benchmark_event_serialize/repeats:16_median                                        228 ns          228 ns           16
benchmark_event_serialize/repeats:16_stddev                                      0.304 ns        0.304 ns           16
benchmark_event_serialize/repeats:16_cv                                           0.13 %          0.13 %            16
benchmark_metric_serialize<Count>/repeats:16_mean                                  222 ns          222 ns           16
benchmark_metric_serialize<Count>/repeats:16_median                                222 ns          222 ns           16
benchmark_metric_serialize<Count>/repeats:16_stddev                              0.403 ns        0.405 ns           16
benchmark_metric_serialize<Count>/repeats:16_cv                                   0.18 %          0.18 %            16
benchmark_metric_serialize<Gauge>/repeats:16_mean                                  197 ns          197 ns           16
benchmark_metric_serialize<Gauge>/repeats:16_median                                196 ns          196 ns           16
benchmark_metric_serialize<Gauge>/repeats:16_stddev                              0.695 ns        0.694 ns           16
benchmark_metric_serialize<Gauge>/repeats:16_cv                                   0.35 %          0.35 %            16
benchmark_metric_serialize<Histogram>/repeats:16_mean                              290 ns          290 ns           16
benchmark_metric_serialize<Histogram>/repeats:16_median                            290 ns          290 ns           16
benchmark_metric_serialize<Histogram>/repeats:16_stddev                          0.601 ns        0.600 ns           16
benchmark_metric_serialize<Histogram>/repeats:16_cv                               0.21 %          0.21 %            16
benchmark_udp_client_send_metric                                                  7342 ns         7342 ns        94870
```

#### GCC-13 Results

```
2024-09-20T12:43:14+00:00
Running ./build/benchmarks/bark_benchmarks
Run on (4 X 3244.6 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x2)
  L1 Instruction 32 KiB (x2)
  L2 Unified 512 KiB (x2)
  L3 Unified 32768 KiB (x1)
Load Average: 1.85, 0.63, 0.24
----------------------------------------------------------------------------------------------------------------------
Benchmark                                                                            Time             CPU   Iterations
----------------------------------------------------------------------------------------------------------------------
benchmark_client_send_metric<Client>/iterations:25000/repeats:32_mean             7752 ns         7752 ns           32
benchmark_client_send_metric<Client>/iterations:25000/repeats:32_median           7722 ns         7722 ns           32
benchmark_client_send_metric<Client>/iterations:25000/repeats:32_stddev            102 ns          102 ns           32
benchmark_client_send_metric<Client>/iterations:25000/repeats:32_cv               1.31 %          1.31 %            32
benchmark_client_send_event<AsioClient>/iterations:25000/repeats:32_mean           255 ns          255 ns           32
benchmark_client_send_event<AsioClient>/iterations:25000/repeats:32_median         254 ns          254 ns           32
benchmark_client_send_event<AsioClient>/iterations:25000/repeats:32_stddev        46.8 ns         46.8 ns           32
benchmark_client_send_event<AsioClient>/iterations:25000/repeats:32_cv           18.39 %         18.40 %            32
benchmark_client_send_metric<SPSCClient>/iterations:25000/repeats:32_mean         70.9 ns         70.9 ns           32
benchmark_client_send_metric<SPSCClient>/iterations:25000/repeats:32_median       69.0 ns         68.9 ns           32
benchmark_client_send_metric<SPSCClient>/iterations:25000/repeats:32_stddev       9.84 ns         9.84 ns           32
benchmark_client_send_metric<SPSCClient>/iterations:25000/repeats:32_cv          13.87 %         13.88 %            32
benchmark_client_send_metric<MPMCClient>/iterations:25000/repeats:32_mean         66.8 ns         66.8 ns           32
benchmark_client_send_metric<MPMCClient>/iterations:25000/repeats:32_median       65.7 ns         65.6 ns           32
benchmark_client_send_metric<MPMCClient>/iterations:25000/repeats:32_stddev       3.46 ns         3.44 ns           32
benchmark_client_send_metric<MPMCClient>/iterations:25000/repeats:32_cv           5.17 %          5.14 %            32
benchmark_client_send_event<Client>/iterations:25000/repeats:32_mean              7690 ns         7689 ns           32
benchmark_client_send_event<Client>/iterations:25000/repeats:32_median            7680 ns         7679 ns           32
benchmark_client_send_event<Client>/iterations:25000/repeats:32_stddev            32.9 ns         33.0 ns           32
benchmark_client_send_event<Client>/iterations:25000/repeats:32_cv                0.43 %          0.43 %            32
benchmark_client_send_metric<AsioClient>/iterations:25000/repeats:32_mean          131 ns          131 ns           32
benchmark_client_send_metric<AsioClient>/iterations:25000/repeats:32_median        128 ns          128 ns           32
benchmark_client_send_metric<AsioClient>/iterations:25000/repeats:32_stddev       11.7 ns         11.7 ns           32
benchmark_client_send_metric<AsioClient>/iterations:25000/repeats:32_cv           8.98 %          8.99 %            32
benchmark_client_send_event<SPSCClient>/iterations:25000/repeats:32_mean          78.2 ns         78.1 ns           32
benchmark_client_send_event<SPSCClient>/iterations:25000/repeats:32_median        77.3 ns         77.3 ns           32
benchmark_client_send_event<SPSCClient>/iterations:25000/repeats:32_stddev        5.55 ns         5.55 ns           32
benchmark_client_send_event<SPSCClient>/iterations:25000/repeats:32_cv            7.10 %          7.10 %            32
benchmark_client_send_event<MPMCClient>/iterations:25000/repeats:32_mean          77.3 ns         77.2 ns           32
benchmark_client_send_event<MPMCClient>/iterations:25000/repeats:32_median        76.1 ns         76.0 ns           32
benchmark_client_send_event<MPMCClient>/iterations:25000/repeats:32_stddev        3.88 ns         3.89 ns           32
benchmark_client_send_event<MPMCClient>/iterations:25000/repeats:32_cv            5.02 %          5.04 %            32
benchmark_event_serialize/repeats:16_mean                                          231 ns          231 ns           16
benchmark_event_serialize/repeats:16_median                                        231 ns          231 ns           16
benchmark_event_serialize/repeats:16_stddev                                       1.16 ns         1.16 ns           16
benchmark_event_serialize/repeats:16_cv                                           0.50 %          0.50 %            16
benchmark_metric_serialize<Count>/repeats:16_mean                                  241 ns          241 ns           16
benchmark_metric_serialize<Count>/repeats:16_median                                240 ns          240 ns           16
benchmark_metric_serialize<Count>/repeats:16_stddev                              0.901 ns        0.903 ns           16
benchmark_metric_serialize<Count>/repeats:16_cv                                   0.37 %          0.38 %            16
benchmark_metric_serialize<Gauge>/repeats:16_mean                                  213 ns          213 ns           16
benchmark_metric_serialize<Gauge>/repeats:16_median                                213 ns          213 ns           16
benchmark_metric_serialize<Gauge>/repeats:16_stddev                               1.18 ns         1.18 ns           16
benchmark_metric_serialize<Gauge>/repeats:16_cv                                   0.55 %          0.55 %            16
benchmark_metric_serialize<Histogram>/repeats:16_mean                              313 ns          313 ns           16
benchmark_metric_serialize<Histogram>/repeats:16_median                            313 ns          313 ns           16
benchmark_metric_serialize<Histogram>/repeats:16_stddev                          0.559 ns        0.560 ns           16
benchmark_metric_serialize<Histogram>/repeats:16_cv                               0.18 %          0.18 %            16
benchmark_udp_client_send_metric                                                  7332 ns         7332 ns        95439
```



## Building and installing

Create a copy of `CMakeUserPresets.template.json` called `CMakeUserPreset.json`. Fix the `VCPKG_ROOT` environment variable in the new preset file, to point to somewhere on your system where you have vcpkg installed. Now you can run with presets `dev` etc.

You might want to remove the `clang-tidy` preset inherit from `dev-common` while developing to **greatly** speed up compilation time.

