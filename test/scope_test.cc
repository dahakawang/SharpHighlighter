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

    SECTION("scope can be compared") {
        ScopeName name1("source.control.c.if");
        ScopeName name2("haha");
        ScopeName name3 = name1;

        REQUIRE(name1 != name2);
        REQUIRE_FALSE(name1 == name2);
        REQUIRE(name1 == name3);
    }
}


TEST_CASE("Scope Test") {
    
    SECTION("scope be constructed from string") {
        Scope s("   source    punctuation.definition.string.end.c        variable.parameter.preprocessor.c  ");

        REQUIRE(s.size() == 3);
        REQUIRE(s[0].name() == "source");
        REQUIRE(s[1].name() == "punctuation.definition.string.end.c");
        REQUIRE(s[2].name() == "variable.parameter.preprocessor.c");
    }

    SECTION("scope name is normalized") {
        Scope s("   source    punctuation.definition.string.end.c        variable.parameter.preprocessor.c  ");

        REQUIRE(s.name() == "source punctuation.definition.string.end.c variable.parameter.preprocessor.c");
    }

    SECTION("scope will be broken up into ScopeNames") {
        Scope s("source punctuation.definition.string.end.c variable.parameter.preprocessor.c");

        REQUIRE(s.size() == 3);
        REQUIRE(s[0].name() == "source");
        REQUIRE(s[1].name() == "punctuation.definition.string.end.c");
    }

    SECTION("scope can be compared") {
        Scope s1("source punctuation.definition.string.end.c variable.parameter.preprocessor.c");
        Scope s2 = s1;
        Scope s3("ource punctuation.definition.string.end.c variable.parameter.preprocessor.c");

        REQUIRE(s1 == s2);
        REQUIRE(s1 != s3);
        REQUIRE_FALSE(s1 != s2);
        REQUIRE_FALSE(s1 == s3);

    }

    SECTION("can check if a scope is prefix for another") {
        Scope prefix("source name level3");
        Scope name("source name level3 name");
        Scope nonprefix("source test");
        Scope empty;

        REQUIRE(name.is_prefix_of(name));
        REQUIRE(prefix.is_prefix_of(name));
        REQUIRE_FALSE(name.is_prefix_of(prefix));
        REQUIRE_FALSE(nonprefix.is_prefix_of(name));
        REQUIRE(empty.is_prefix_of(name));
        REQUIRE(empty.is_prefix_of(prefix));
        REQUIRE(empty.is_prefix_of(nonprefix));
        REQUIRE(empty.is_prefix_of(empty));
    }

    SECTION("can extract subscopes from Scope") {
        Scope s("abc def gh fij");

        REQUIRE(s.subscope(0) == Scope("abc def gh fij"));
        REQUIRE(s.subscope(2) == Scope("gh fij"));
        REQUIRE(s.subscope(3) == Scope("fij"));
        REQUIRE(s.subscope(4) == Scope(""));
    }

    SECTION("scope name can be expanded") {
        string text("Hello");
        Match m = Match::make_dummy(0, 5);
        Scope s("source.makefile keyword.control.$0", text, m);

        REQUIRE( s[1].name()== "keyword.control.Hello");

        Scope s1("source.makefile keyword.control.${0}", text, m);
        REQUIRE( s1[1].name()== "keyword.control.Hello");

        Scope s2("source.makefile keyword.control.${0:/downcase}", text, m);
        REQUIRE( s2[1].name()== "keyword.control.hello");

        Scope s3("source.makefile keyword.control.${0:/upcase}", text, m);
        REQUIRE( s3[1].name()== "keyword.control.HELLO");

    }

}
