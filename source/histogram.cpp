#include <string>

#include "twig/datadog/histogram.hpp"

#include "./serialize.hpp"
#include "twig/datadog/tags.hpp"

namespace twig::datadog
{

auto Histogram::serialize(const Tags& global_tags) const -> std::string
{
    return twig::datadog::serialize(*this, global_tags);
}

}  // namespace twig::datadog
