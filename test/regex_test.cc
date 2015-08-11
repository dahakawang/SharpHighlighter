//#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <regex.h>

using namespace shl;

TEST_CASE("Range Test") {
    SECTION("default constructs a (0,0) Range") {
        Range range;

        REQUIRE(range.position == 0);
        REQUIRE(range.length == 0);
    }

    SECTION("constructs a Range by specify position&length") {
        Range range(100, 23);

        REQUIRE(range.position == 100);
        REQUIRE(range.length == 23);
    }

    SECTION("constructs by copy a range") {
        Range range2(10,20);
        Range range(range2);

        REQUIRE(range.position == 10);
        REQUIRE(range.length == 20);
    }

    SECTION("the Range object is assignable") {
        Range range, range2(10,20);
        range = range2;

        REQUIRE(range.position == 10);
        REQUIRE(range.length == 20);
    }

    SECTION("the end of Range is position plus length") {
        Range range(25, 456);

        REQUIRE(range.end() == (range.position + range.length));
    }

    SECTION("Range can be compared equal/inequal") {
        Range range1(25, 30), range2(25, 30), range3(25, 31), range4(24, 30);

        REQUIRE(range1 == range2);
        REQUIRE(range1 == range1);
        REQUIRE_FALSE(range1 == range3);
        REQUIRE_FALSE(range1 == range4);
        REQUIRE(range1 != range3);
        REQUIRE(range1 != range4);
    }
}



TEST_CASE("Match Tests") {

    SECTION("default constructor creates a no match object") {
        Match match;
        REQUIRE_FALSE((bool)match);
        REQUIRE( match == Match::NOT_MATCHED );
        REQUIRE(match.size() == 0);
    }
    
    SECTION("dummy Match object can be created by make_dummy") {
        Match match = Match::make_dummy(0, 10);

        REQUIRE((bool)match);
        REQUIRE(match == Match::MATCHED);
        REQUIRE(match.size() == 1);
        REQUIRE(match[0] == Range(0, 10));
    }
}

TEST_CASE("Regex can find matchs") {

    SECTION("find a simple match") {
        Regex r("\\d{3}-\\d{4}");
        auto result = r.match("hello my Tel. is 322-0592", 0);

        REQUIRE((bool)result);
        REQUIRE( result == Match::MATCHED );
    }
}
