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

        REQUIRE(obj.file_types.size() == 2);
        REQUIRE(obj.file_types[0] == "c");
        REQUIRE(obj.file_types[1] == "h");

        REQUIRE(obj.patterns.size() == 2);
        REQUIRE(obj.patterns[0].include == "#preprocessor-rule-enabled");
        REQUIRE(obj.patterns[1].begin == "\"");
        REQUIRE(obj.patterns[1].end == "\"");
        REQUIRE(obj.patterns[1].name == "string.quoted.double.c");
        REQUIRE(obj.patterns[1].begin_captures.size() == 1);
        REQUIRE(obj.patterns[1].begin_captures["0"]["name"]  == "punctuation.definition.string.begin.c");
        REQUIRE(obj.patterns[1].end_captures.size() == 1);
        REQUIRE(obj.patterns[1].end_captures["0"]["name"]  == "punctuation.definition.string.end.c");
        REQUIRE(obj.patterns[1].patterns.size() == 3);
        REQUIRE(obj.patterns[1].patterns[2].include == "$base");

        REQUIRE(obj.repository.size() == 3);
        REQUIRE(obj.repository["preprocessor-rule-enabled"].begin == "^\\s*(#(if)\\s+(0*1)\\b)");
        REQUIRE(obj.repository["preprocessor-rule-enabled"].patterns[0].content_name == "comment.block.preprocessor.else-branch");

        REQUIRE(obj.injections.size() == 1);
        REQUIRE(obj.injections["hello"].patterns.size() == 1);
    }
}
