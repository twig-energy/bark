#pragma once

#include <cstdint>
#include <string>
#include <utility>

#include "bark/tags.hpp"

namespace bark
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

    constexpr auto with(Tags tags_) -> Event&
    {
        this->tags = std::move(tags_);
        return *this;
    }

    constexpr auto with(Priority priority_) -> Event&
    {
        this->priority = priority_;
        return *this;
    }

    constexpr auto with(AlertType alert_type_) -> Event&
    {
        this->alert_type = alert_type_;
        return *this;
    }

    auto serialize(const Tags& global_tags) const -> std::string;
};

}  // namespace bark
