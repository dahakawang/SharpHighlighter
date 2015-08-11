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

}
