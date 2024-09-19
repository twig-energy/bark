#include <string>

#include "twig/datadog/event.hpp"

#include "./serialize.hpp"
#include "twig/datadog/tags.hpp"

namespace twig::datadog
{

auto Event::serialize(const Tags& global_tags) const -> std::string
{
    return twig::datadog::serialize(*this, global_tags);
}

}  // namespace twig::datadog
