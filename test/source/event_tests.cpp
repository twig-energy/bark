#include <utility>

#include "twig/datadog/event.hpp"

#include <doctest/doctest.h>

#include "twig/datadog/tags.hpp"

namespace twig::datadog
{

TEST_SUITE("Event")
{
    TEST_CASE("Can format event with no tags")
    {
        auto event = Event("event", "some data here");
        CHECK_EQ("_e{5,14}:event|some data here", event.serialize());
    }

    TEST_CASE("Can format event with tags")
    {
        auto tags = Tags::from_tags({"tag1:hello", "tag2:world"});

        auto event = Event("event", "a description here").with(std::move(tags));
        CHECK_EQ("_e{5,18}:event|a description here|#tag1:hello,tag2:world", event.serialize());
    }

    TEST_CASE("Can format event with tags and alert type")
    {
        auto tags = Tags::from_tags({"tag1:hello", "tag2:world"});

        auto event = Event("event", "a description here").with(std::move(tags)).with(AlertType::ERROR);
        CHECK_EQ("_e{5,18}:event|a description here|t:error|#tag1:hello,tag2:world", event.serialize());
    }

    TEST_CASE("Can format event with priority")
    {
        auto event = Event("event", "a description here").with(Priority::LOW);
        CHECK_EQ("_e{5,18}:event|a description here|p:low", event.serialize());
    }
}

}  // namespace twig::datadog
