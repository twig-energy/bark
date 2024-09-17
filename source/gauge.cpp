#include <string>

#include "twig/datadog/gauge.hpp"

#include <fmt/format.h>

#include "./fmt.hpp"

namespace twig::datadog
{

auto Gauge::serialize() const -> std::string
{
    return fmt::format("{}", *this);
}

}  // namespace twig::datadog
