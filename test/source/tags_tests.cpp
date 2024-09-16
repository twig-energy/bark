#include <array>
#include <string_view>
#include <utility>

#include "twig/datadog/tags.hpp"

#include <doctest/doctest.h>

namespace twig::datadog
{

TEST_SUITE("Tags")
{
    TEST_CASE("Can serialize empty tags")
    {
        auto tags = Tags {};
        CHECK_EQ("", tags.str());

        tags = Tags::create(std::array<std::string_view, 0> {});
        CHECK_EQ("", tags.str());

        tags = Tags::create(std::array<std::pair<std::string_view, std::string_view>, 0> {});
        CHECK_EQ("", tags.str());
    }

    TEST_CASE("Can serialize single tag")
    {
        auto tags = Tags::create(std::array<std::string_view, 1> {"tag:value"});
        CHECK_EQ("tag:value", tags.str());

        tags = Tags::create(std::array<std::pair<std::string_view, std::string_view>, 1> {
            std::pair<std::string_view, std::string_view> {"tag", "value"}});
        CHECK_EQ("tag:value", tags.str());
    }

    TEST_CASE("Can serialize two tags")
    {
        auto tags = Tags::create(std::array<std::string_view, 2> {"tag:value", "other_tag:other_value"});
        CHECK_EQ("tag:value,other_tag:other_value", tags.str());

        tags = Tags::create(std::array<std::pair<std::string_view, std::string_view>, 2> {
            std::pair<std::string_view, std::string_view> {"tag", "value"}, {"other_tag", "other_value"}});
        CHECK_EQ("tag:value,other_tag:other_value", tags.str());
    }
}

}  // namespace twig::datadog
