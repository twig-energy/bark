#include "twig/datadog/count.hpp"

#include <fmt/format.h>

#include "./fmt.hpp"

namespace twig::datadog
{

auto Count::serialize() const -> std::string
{
    return fmt::format("{}", *this);
}

}  // namespace twig::datadog
