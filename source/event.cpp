#include <string>

#include "twig/datadog/event.hpp"

#include <fmt/format.h>

#include "./fmt.hpp"

namespace twig::datadog
{

auto Event::serialize() const -> std::string
{
    return fmt::format("{}", *this);
}

}  // namespace twig::datadog
