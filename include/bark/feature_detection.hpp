#pragma once

#include <asio/detail/config.hpp>

// NOLINTBEGIN(cppcoreguidelines-macro-usage,cppcoreguidelines-macro-to-enum,modernize-macro-to-enum)

#if defined(__clang__) && !defined(__ibmxl__)
#    define BARK_CLANG_VERSION (__clang_major__ * 100 + __clang_minor__)
#else
#    define BARK_CLANG_VERSION 0
#endif

#if defined(__GNUC__) && !defined(__clang__) && !defined(__INTEL_COMPILER)
#    define BARK_GCC_VERSION (__GNUC__ * 100 + __GNUC_MINOR__)
#else
#    define BARK_GCC_VERSION 0
#endif

#if defined(__ICL)
#    define BARK_ICC_VERSION __ICL
#elif defined(__INTEL_COMPILER)
#    define BARK_ICC_VERSION __INTEL_COMPILER
#else
#    define BARK_ICC_VERSION 0
#endif

#if defined(_MSC_VER)
#    define BARK_MSC_VERSION _MSC_VER
#else
#    define BARK_MSC_VERSION 0
#endif

#ifdef BARK_USE_CONSTEXPR
// Use the provided definition.
#elif BARK_CLANG_VERSION >= 1500
#    define BARK_USE_CONSTEXPR 1
#elif BARK_GCC_VERSION >= 1200
// constexpr std::string only supported by g++-12 or later.
#    define BARK_USE_CONSTEXPR 1
#elif BARK_ICC_VERSION
#    define BARK_USE_CONSTEXPR 0
#elif BARK_MSC_VERSION >= 1912
#    define BARK_USE_CONSTEXPR 1
#else
#    define BARK_USE_CONSTEXPR 0
#endif

#if BARK_USE_CONSTEXPR
#    define BARK_CONSTEXPR constexpr
#else
#    define BARK_CONSTEXPR
#endif

#if ASIO_HAS_LOCAL_SOCKETS && BARK_MSC_VERSION == 0
#    define BARK_UDS_ENABLED 1
#else
#    define BARK_UDS_ENABLED 0
#endif

// NOLINTEND(cppcoreguidelines-macro-usage,cppcoreguidelines-macro-to-enum,modernize-macro-to-enum)
