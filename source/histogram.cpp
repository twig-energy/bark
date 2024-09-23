#include <string>

#include "bark/histogram.hpp"

#include "./serialize.hpp"
#include "bark/tags.hpp"

namespace bark
{

auto Histogram::serialize(const Tags& global_tags) const -> std::string
{
    return bark::serialize(*this, global_tags);
}

}  // namespace bark
