#pragma once

#include <concepts>
#include <string>
#include <string_view>

#include <fmt/base.h>
#include <fmt/format.h>

#include "bark/event.hpp"
#include "bark/tags.hpp"

namespace bark
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
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
    auto out = fmt::memory_buffer {};
    // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
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
    // NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

    auto iterator = fmt::appender {out};

    iterator = fmt::format_to(iterator, "{}:{}|{}", metric.metric, metric.value, T::metric_type_indicator);
    if constexpr (bark::has_sample_rate<T>) {
        if (metric.sample_rate != 1.0) {
            iterator = fmt::format_to(iterator, "|@{}", metric.sample_rate);
        }
    }
    if (!metric.tags.str().empty()) {
        iterator = fmt::format_to(iterator, "|#{}", metric.tags.str());

        if (!global_tags.str().empty()) {
            iterator = fmt::format_to(iterator, ",{}", global_tags.str());
        }
    } else if (!global_tags.str().empty()) {
        iterator = fmt::format_to(iterator, "|#{}", global_tags.str());
    }

    return fmt::to_string(out);
#pragma GCC diagnostic pop
}

constexpr auto serialize(const Event& event, const Tags& global_tags) -> std::string
{
    auto out = fmt::memory_buffer {};
    // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    out.reserve(3 + 10 + 1 + 10 + 2 +                                   // _e{<title size>,<text size>}:
                event.title.size() + 1 + event.text.size() +            // <title>|<text>
                (event.priority == bark::Priority::LOW ? 5 : 0) +       // |p:low
                (event.alert_type != bark::AlertType::INFO ? 10 : 0) +  // |t:<type>
                event.tags.str().size() + global_tags.str().size() + 2  // |#<tags>
    );
    // NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    auto iterator = fmt::appender {out};

    iterator = fmt::format_to(
        iterator, "_e{{{},{}}}:{}|{}", event.title.length(), event.text.length(), event.title, event.text);

    if (event.priority == bark::Priority::LOW) {
        iterator = fmt::format_to(iterator, "|p:low");
    }

    using enum bark::AlertType;
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

    if (!event.tags.str().empty()) {
        iterator = fmt::format_to(iterator, "|#{}", event.tags.str());

        if (!global_tags.str().empty()) {
            iterator = fmt::format_to(iterator, ",{}", global_tags.str());
        }
    } else if (!global_tags.str().empty()) {
        iterator = fmt::format_to(iterator, "|#{}", global_tags.str());
    }

    return fmt::to_string(out);
}

}  // namespace bark
