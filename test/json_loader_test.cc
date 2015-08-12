#include "catch.hpp"
#include "util.h"
#include <json_loader.h>
#include <iostream>

using namespace shl;

TEST_CASE("JsonLoader Test") {
    string data = load_string("fixture/valid.json");

    SECTION("can load a valid syntax file") {

        JsonLoader loader;
        JsonObject obj = loader.load(data);

        REQUIRE(obj.scope_name == "source.c");
        REQUIRE(obj.name == "C");

    }
}
