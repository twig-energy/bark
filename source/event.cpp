#include <string>

#include "bark/event.hpp"

#include "./serialize.hpp"
#include "bark/tags.hpp"

namespace bark
{

auto Event::serialize(const Tags& global_tags) const -> std::string
{
    return bark::serialize(*this, global_tags);
}

}  // namespace bark
