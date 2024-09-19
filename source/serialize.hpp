#pragma once

#include <concepts>
#include <string>
#include <string_view>

#include <fmt/base.h>
#include <fmt/format.h>

#include "twig/datadog/event.hpp"
#include "twig/datadog/tags.hpp"

namespace twig::datadog
{

template<typename T>
concept has_sample_rate = requires(T t) {
    {
        t.sample_rate
    } -> std::convertible_to<double>;
};

template<typename T>
concept is_metric_like = requires(T t) {
    {
        t.metric
    } -> std::convertible_to<std::string_view>;
    {
        t.value
    } -> std::convertible_to<double>;
    {
        t.tags
    } -> std::convertible_to<Tags>;
    {
        T::metric_type_indicator
    } -> std::convertible_to<std::string_view>;
};

template<is_metric_like T>
constexpr auto serialize(const T& metric, const Tags& global_tags) -> std::string
{
    auto out = fmt::memory_buffer {};
    if constexpr (has_sample_rate<T>) {
        out.reserve(metric.metric.size() + 1 +                               // metric_name:
                    23 +                                                     // value|<indicator>
                    (metric.sample_rate != 1.0 ? 5 : 0) +                    // |@<sample_rate>
                    metric.tags.str().size() + global_tags.str().size() + 2  // |#<tags>
        );
    } else {
        out.reserve(metric.metric.size() + 1 +                               // metric_name:
                    23 +                                                     // value|<indicator>
                    metric.tags.str().size() + global_tags.str().size() + 2  // |#<tags>
        );
    }

    auto iterator = fmt::appender {out};

    iterator = fmt::format_to(iterator, "{}:{}|{}", metric.metric, metric.value, T::metric_type_indicator);
    if constexpr (twig::datadog::has_sample_rate<T>) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
        if (metric.sample_rate != 1.0) {
            iterator = fmt::format_to(iterator, "|@{}", metric.sample_rate);
        }
#pragma GCC diagnostic pop
    }
    if (!metric.tags.str().empty() || !global_tags.str().empty()) {
        iterator = fmt::format_to(iterator, "|#{}{}", metric.tags.str(), global_tags.str());
    }

    return fmt::to_string(out);
}

constexpr auto serialize(const Event& event, const Tags& global_tags) -> std::string
{
    auto out = fmt::memory_buffer {};
    out.reserve(3 + 10 + 1 + 10 + 2 +                                            // _e{<title size>,<text size>}:
                event.title.size() + 1 + event.text.size() +                     // <title>|<text>
                (event.priority == twig::datadog::Priority::LOW ? 5 : 0) +       // |p:low
                (event.alert_type != twig::datadog::AlertType::INFO ? 10 : 0) +  // |t:<type>
                event.tags.str().size() + global_tags.str().size() + 2           // |#<tags>
    );
    auto iterator = fmt::appender {out};

    iterator = fmt::format_to(
        iterator, "_e{{{},{}}}:{}|{}", event.title.length(), event.text.length(), event.title, event.text);

    if (event.priority == twig::datadog::Priority::LOW) {
        iterator = fmt::format_to(iterator, "|p:low");
    }

    using enum twig::datadog::AlertType;
    switch (event.alert_type) {
        case ERROR:
            iterator = fmt::format_to(iterator, "|t:error");
            break;
        case WARNING:
            iterator = fmt::format_to(iterator, "|t:warning");
            break;
        case INFO:
            // This is assumed by default.
            break;
        case SUCCESS:
            iterator = fmt::format_to(iterator, "|t:success");
            break;
    }

    if (!event.tags.str().empty() || !global_tags.str().empty()) {
        iterator = fmt::format_to(iterator, "|#{}{}", event.tags.str(), global_tags.str());
    }

    return fmt::to_string(out);
}

}  // namespace twig::datadog
