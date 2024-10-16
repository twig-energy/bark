#pragma once

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

// Detect standard library versions.
#ifdef _GLIBCXX_RELEASE
#    define BARK_GLIBCXX_RELEASE _GLIBCXX_RELEASE
#else
#    define BARK_GLIBCXX_RELEASE 0
#endif
#ifdef _LIBCPP_VERSION
#    define BARK_LIBCPP_VERSION _LIBCPP_VERSION
#else
#    define BARK_LIBCPP_VERSION 0
#endif

#ifdef _MSVC_LANG
#    define BARK_CPLUSPLUS _MSVC_LANG
#else
#    define BARK_CPLUSPLUS __cplusplus
#endif

// Detect __has_*.
#ifdef __has_feature
#    define BARK_HAS_FEATURE(x) __has_feature(x)
#else
#    define BARK_HAS_FEATURE(x) 0
#endif

// Detect C++14 relaxed constexpr.
#ifdef BARK_USE_CONSTEXPR
// Use the provided definition.
#elif BARK_GCC_VERSION >= 1200
// constexpr std::string only supported by g++-12 or later.
#    define BARK_USE_CONSTEXPR 1
#elif BARK_ICC_VERSION
#    define BARK_USE_CONSTEXPR 0  // https://github.com/fmtlib/fmt/issues/1628
#elif BARK_HAS_FEATURE(cxx_relaxed_constexpr) || BARK_MSC_VERSION >= 1912
#    define BARK_USE_CONSTEXPR 1
#else
#    define BARK_USE_CONSTEXPR 0
#endif
#if BARK_USE_CONSTEXPR
#    define BARK_CONSTEXPR constexpr
#else
#    define BARK_CONSTEXPR
#endif

#if BARK_USE_CONSTEXPR
#    define BARK_CONSTEXPR_OR_INLINE constexpr
#else
#    define BARK_CONSTEXPR_OR_INLINE inline
#endif
