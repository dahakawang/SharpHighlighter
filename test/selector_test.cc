#include "catch.hpp"
#include "util.h"
#include <iostream>
#include <selector_parser.h>

using namespace shl;

TEST_CASE("Selector Parser Test") {
    SECTION("parse a simple selector") {
        selector::Parser parser("source.c storage.modifier.c keyword.core.control.if highlight.*");

        parser.parse_selector();
    }
}
