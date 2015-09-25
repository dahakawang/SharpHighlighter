#include "catch.hpp"
#include "util.h"
#include <shl_exception.h>
#include <grammar_compiler.h>

using namespace shl;

TEST_CASE("GrammarCompiler Test") {
    string data = load_string("fixture/valid.json");
    GrammarCompiler compiler;
    Grammar g = compiler.compile(data);
    compiler.resolve_include(g, nullptr); // do not check external grammar include here

    SECTION("can load a valid grammar") {
        REQUIRE(g.desc == "C");
        REQUIRE(g.file_types.size() == 2);
        REQUIRE(g.name == "source.c");

        REQUIRE(g.repository.size() == 3);
        REQUIRE(g.repository["preprocessor-rule-enabled"].begin.source() == "^\\s*(#(if)\\s+(0*1)\\b)");
    }

    SECTION("can load an enclosing rule") {
        REQUIRE(g.repository.find("hello") != g.repository.end());

        auto object = g.repository["hello"];
        REQUIRE(object.patterns.size() == 1);
        REQUIRE(object.patterns[0].patterns.size() == 2);
    }

    SECTION("captures can be regard as begin_captures if is a begin/end rule and has no begin_captures") {
        REQUIRE(g.repository["preprocessor-rule-enabled"].begin_captures.size() == 3);
        REQUIRE(g.repository["preprocessor-rule-enabled"].begin_captures[2] == "keyword.control.import.if.c");
    }

    SECTION("captures can be regard as end_captures if is a begin/end rule and has no end_captures") {
        REQUIRE(g.repository["preprocessor-rule-enabled"].end_captures.size() == 3);
        REQUIRE(g.repository["preprocessor-rule-enabled"].end_captures[2] == "keyword.control.import.if.c");
    }

    SECTION("match rule merged with begin, and captures merge with begin_captures") {
        REQUIRE(g.repository["string_escaped_char"].patterns[1].begin.source() == "\\\\.");
        REQUIRE(g.patterns[1].begin_captures.size() == 1);
        REQUIRE(g.patterns[1].begin.source() == "\"");
        REQUIRE(g.repository["preprocessor-rule-enabled"].begin.source() == "^\\s*(#(if)\\s+(0*1)\\b)");
        REQUIRE(g.repository["preprocessor-rule-enabled"].begin_captures.size() == 3);
    }


    SECTION("can resolve includes") {
        REQUIRE(g.patterns[0].include.ptr == &g.repository["preprocessor-rule-enabled"]);
        REQUIRE(g.patterns[1].patterns[0].include.ptr == &g.repository["string_escaped_char"]);
        REQUIRE(g.patterns[1].patterns[1].include.ptr == &g);
        REQUIRE(g.patterns[1].patterns[2].include.ptr == nullptr);
        REQUIRE(g.patterns[1].patterns[2].include.is_base_ref == true);

        REQUIRE(g.repository["preprocessor-rule-enabled"].patterns[0].patterns[0].include.ptr == &g);
        REQUIRE(g.repository["preprocessor-rule-enabled"].patterns[0].patterns[1].include.ptr == &g.repository["string_escaped_char"]);
        REQUIRE(g.repository["preprocessor-rule-enabled"].patterns[1].patterns[0].include.ptr == nullptr);
        REQUIRE(g.repository["preprocessor-rule-enabled"].patterns[1].patterns[0].include.is_base_ref == true);

        REQUIRE(g.repository["hello"].patterns[0].patterns[0].include.ptr == &g.repository["preprocessor-rule-enabled"]);
        REQUIRE(g.repository["hello"].patterns[0].patterns[1].include.ptr == &g.repository["hello"]);
    }

    SECTION("will throw if encounter a external grammar include but GrammarRegister is not set") {
        string data = load_string("fixture/ruby.json");
        GrammarCompiler compiler;
        Grammar g = compiler.compile(data);
        
        REQUIRE_THROWS_AS( compiler.resolve_include(g, nullptr), ShlException);
    }

    SECTION("will load the injection rule") {
        REQUIRE( g.injections.size() == 1 );
    }
}
