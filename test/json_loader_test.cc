#include "catch.hpp"
#include "util.h"
#include <json_loader.h>
#include <iostream>

using namespace shl;

TEST_CASE("JsonLoader Test") {
    SECTION("can load a valid syntax file") {
        string data = load_string("fixture/valid.json");

        JsonLoader loader;
        JsonObject obj = loader.load(data);
    }
}
