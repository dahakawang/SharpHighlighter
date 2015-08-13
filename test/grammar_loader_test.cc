#include "catch.hpp"
#include "util.h"
#include <grammar_loader.h>

using namespace shl;

TEST_CASE("GrammarLoader Test") {
    string data = load_string("fixture/valid.json");
    GrammarLoader loader;
    Grammar g = loader.load(data);

    SECTION("can load a valid grammar") {
        REQUIRE(g.desc == "C");
        REQUIRE(g.file_types.size() == 2);
        REQUIRE(g.name == "source.c");

        REQUIRE(g.repository.size() == 2);
        REQUIRE(g.repository["preprocessor-rule-enabled"].begin.source() == "^\\s*(#(if)\\s+(0*1)\\b)");
    }

    SECTION("unused repository is ignored") {
        REQUIRE(g.repository.size() == 2);
    }

    SECTION("captures can be regard as begin_captures if is a begin/end rule and has no begin_captures") {
        REQUIRE(g.repository["preprocessor-rule-enabled"].begin_captures.size() == 3);
        REQUIRE(g.repository["preprocessor-rule-enabled"].begin_captures[2] == "keyword.control.import.if.c");
    }

    SECTION("match rule merged with begin, and captures merge with begin_captures") {
        REQUIRE(g.repository["string_escaped_char"].patterns[1].begin.source() == "\\\\.");
        REQUIRE(g.patterns[1].begin_captures.size() == 1);
        REQUIRE(g.patterns[1].begin.source() == "\"");
        REQUIRE(g.repository["preprocessor-rule-enabled"].begin.source() == "^\\s*(#(if)\\s+(0*1)\\b)");
        REQUIRE(g.repository["preprocessor-rule-enabled"].begin_captures.size() == 3);
    }


    SECTION("can resolve includes") {
        REQUIRE(g.patterns[0].include == &g.repository["preprocessor-rule-enabled"]);
        REQUIRE(g.patterns[1].patterns[0].include == &g.repository["string_escaped_char"]);
        REQUIRE(g.patterns[1].patterns[1].include == &g.patterns[1]);
        REQUIRE(g.patterns[1].patterns[2].include == &g);

        REQUIRE(g.repository["preprocessor-rule-enabled"].patterns[0].patterns[0].include == &g.repository["preprocessor-rule-enabled"].patterns[0]);
        REQUIRE(g.repository["preprocessor-rule-enabled"].patterns[0].patterns[1].include == &g.repository["string_escaped_char"]);
        REQUIRE(g.repository["preprocessor-rule-enabled"].patterns[1].patterns[0].include == &g);
    }
}
