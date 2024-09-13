#include <string>
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

namespace twig::products
{

// doctest::detail::g_cs->currentTest->m_name is only accessible in the compilation unit that has
// DOCTEST_CONFIG_IMPLEMENT or DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN defined.
auto current_test_name() -> std::string
{
    return doctest::detail::g_cs->currentTest->m_name;
}

}  // namespace twig::products
