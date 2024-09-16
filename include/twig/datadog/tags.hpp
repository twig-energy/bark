#pragma once

#include <cstddef>
#include <span>
#include <string>
#include <string_view>
#include <utility>

namespace twig::datadog
{

namespace
{

constexpr auto tags_str_length(std::span<const std::pair<std::string_view, std::string_view>> tags) -> std::size_t
{
    auto length = std::size_t {tags.size() - 1};  // commas between tags
    for (const auto& [key, value] : tags) {
        length += key.size() + value.size() + 1;  // +1 for colons between key-value-pairs
    }
    return length;
}

constexpr auto tags_str_length(std::span<const std::string_view> tags) -> std::size_t
{
    auto length = std::size_t {tags.size() - 1};  // commas between tags
    for (const auto& tag : tags) {
        length += tag.size();
    }
    return length;
}

}  // namespace

class Tags
{
    std::string _tags;

  public:
    constexpr Tags() = default;
    constexpr explicit Tags(std::string tags)
        : _tags(std::move(tags))
    {
    }

    constexpr static auto create(std::span<const std::pair<std::string_view, std::string_view>> tags) -> Tags
    {
        if (tags.empty()) {
            return Tags {};
        }

        auto tags_string = std::string {};
        tags_string.reserve(tags_str_length(tags) + 1);

        for (auto i = std::size_t {0}; i < tags.size(); i++) {
            const auto& [key, value] = tags[i];
            tags_string.append(key);
            tags_string.push_back(':');
            tags_string.append(value);
            tags_string.push_back(',');
        }

        tags_string.pop_back();  // Remove trailing comma.

        return Tags {std::move(tags_string)};
    }

    constexpr static auto create(std::span<const std::string_view> tags) -> Tags
    {
        if (tags.empty()) {
            return Tags {};
        }

        auto tags_string = std::string {};
        tags_string.reserve(tags_str_length(tags) + 1);

        for (auto i = std::size_t {0}; i < tags.size(); i++) {
            const auto& tag = tags[i];
            tags_string.append(tag);
            tags_string.push_back(',');
        }

        tags_string.pop_back();  // Remove trailing comma.

        return Tags {std::move(tags_string)};
    }

    constexpr auto str() const -> const std::string&
    {
        return this->_tags;
    }
};

constexpr const auto no_tags = Tags();

}  // namespace twig::datadog
