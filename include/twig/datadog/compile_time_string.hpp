#pragma once

#include <string_view>

namespace twig::datadog
{

struct CompileTimeString
{
    std::string_view value;

    consteval CompileTimeString(std::string_view string_view)
        : value(string_view)
    {
    }

    consteval CompileTimeString(const char* raw_string)
        : value(raw_string)
    {
    }

    constexpr operator std::string_view() const
    {
        return value;
    }
};

}  // namespace twig::datadog
