#include "twig/datadog/histogram.hpp"

#include <fmt/format.h>

#include "./fmt.hpp"

namespace twig::datadog
{

auto Histogram::serialize() const -> std::string
{
    return fmt::format("{}", *this);
}

}  // namespace twig::datadog
