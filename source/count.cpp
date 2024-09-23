#include <string>

#include "bark/count.hpp"

#include "bark/tags.hpp"
#include "serialize.hpp"

namespace bark
{

auto Count::serialize(const Tags& global_tags) const -> std::string
{
    return bark::serialize(*this, global_tags);
}

}  // namespace bark
