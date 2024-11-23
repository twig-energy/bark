#pragma once

#include <concepts>
#include <string>
#include <string_view>
#include <utility>

namespace bark
{

template<typename T>
concept sync_datagram_transport = requires(T t) {
    {
        t.send(std::declval<std::string_view>())
    } -> std::same_as<bool>;
};

template<typename T>
concept async_datagram_transport = requires(T t) {
    {
        t.send_async(std::declval<std::string&&>())
    } -> std::same_as<void>;
};

template<typename T>
concept datagram_transport = sync_datagram_transport<T> || async_datagram_transport<T>;

}  // namespace bark
