//#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <regex.h>

using namespace shl;

TEST_CASE("Regex can find matchs") {
    SECTION("find a simple match") {
        Regex r("\\d{3}-\\d{4}");
        auto result = r.match("hello my Tel. is 322-0592", 0);
    }
}
