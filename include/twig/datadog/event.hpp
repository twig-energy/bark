#pragma once

#include <cstdint>
#include <string>
#include <utility>

#include "twig/datadog/tags.hpp"

namespace twig::datadog
{

enum class Priority : uint8_t
{
    NORMAL,
    LOW,
};

enum class AlertType : uint8_t
{
    ERROR,
    WARNING,
    INFO,
    SUCCESS,
};

struct Event
{
    std::string title;
    std::string text;
    // TODO: Timestamp
    // TODO: Hostname
    // TODO: Aggregation key
    Priority priority = Priority::NORMAL;
    // TODO: Source type name
    AlertType alert_type = AlertType::INFO;
    Tags tags;

    constexpr Event(std::string title_, std::string text_) noexcept
        : title(std::move(title_))
        , text(std::move(text_))
    {
    }

    constexpr auto with_tags(Tags tags_) -> Event&
    {
        tags = std::move(tags_);
        return *this;
    }

    constexpr auto with_priority(Priority priority_) -> Event&
    {
        priority = priority_;
        return *this;
    }

    constexpr auto with_alert_type(AlertType alert_type_) -> Event&
    {
        alert_type = alert_type_;
        return *this;
    }

    auto serialize() const -> std::string;
};

}  // namespace twig::datadog
