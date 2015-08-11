#include "catch.hpp"
#include <scope.h>

using namespace shl;

TEST_CASE("Scope Name Test") {

    SECTION("default contructor create a empty ScopeName") {
        ScopeName name;

        REQUIRE(name.name() == "");
        REQUIRE(name.breakdown().size() == 0);
    }

    SECTION("scope name can be constructed by string") {
        ScopeName name("source.control.c.if");

        REQUIRE(name.name() == "source.control.c.if");

        vector<string> elem = {"source", "control", "c", "if"};
        REQUIRE(name.breakdown().size() == elem.size());
        for (unsigned i = 0; i < elem.size(); i++) {
            REQUIRE(name.breakdown()[i] == elem[i]);
        }
    }

    SECTION("a empty ScopeName can be contructed by \"\"") {
        ScopeName name("");

        REQUIRE(name.name() == "");
        REQUIRE(name.breakdown().size() == 0);
    }
}
