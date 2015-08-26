//#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <shl_regex.h>
#include <shl_exception.h>

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

    SECTION("contain can check if range contains another") {
        Range r1(100, 100), r2(0, 10), r3(0, 100), r4(0, 101), r5(0, 150);
        Range r6(0, 200), r7(100, 50), r8(100, 100), r9(100, 101);
        Range r10(150, 50), r11(150, 51), r12(200, 10), r13(201, 100);

        REQUIRE_FALSE( r1.contain(r2) );
        REQUIRE_FALSE( r1.contain(r3) );
        REQUIRE_FALSE( r1.contain(r4) );
        REQUIRE_FALSE( r1.contain(r5) );
        REQUIRE_FALSE( r1.contain(r6) );
        REQUIRE( r1.contain(r7) );
        REQUIRE( r1.contain(r8) );
        REQUIRE_FALSE( r1.contain(r9) );
        REQUIRE( r1.contain(r10) );
        REQUIRE_FALSE( r1.contain(r11) );
        REQUIRE_FALSE( r1.contain(r12) );
        REQUIRE_FALSE( r1.contain(r13) );
    }
}



TEST_CASE("Match Tests") {

    SECTION("match can be checked equality with MatcheRsult") {
        Match match;

        REQUIRE(match == Match::NOT_MATCHED);
        REQUIRE(Match::NOT_MATCHED == match);
        REQUIRE_FALSE(Match::MATCHED == match);
        REQUIRE(Match::MATCHED != match);
        REQUIRE(match != Match::MATCHED);
    }

    SECTION("default constructor creates a no match object") {
        Match match;
        REQUIRE( match == Match::NOT_MATCHED );
        REQUIRE(match.size() == 0);
    }
    
    SECTION("dummy Match object can be created by make_dummy") {
        Match match = Match::make_dummy(0, 10);

        REQUIRE(match == Match::MATCHED);
        REQUIRE(match.size() == 1);
        REQUIRE(match[0] == Range(0, 10));
    }
}

TEST_CASE("Regex can find matchs") {

    SECTION("find a simple match") {
        Regex r("\\d{3}-\\d{4}");
        auto result = r.match("hello my Tel. is 322-0592", 0);

        REQUIRE( result == Match::MATCHED );
    }

    SECTION("capture whole regex by default") {
        Regex r("\\d{3}-\\d{4}");
        string target = "hello my Tel. is 322-0592";
        auto result = r.match(target, 0);

        REQUIRE(result.size() == 1);
        REQUIRE(result[0] == Range(17, 8));
        REQUIRE(result[0].substr(target) == "322-0592");
    }

    SECTION("regex can capture groups") {
        Regex r("(\\d{3})-(\\d{4})");
        string target = "hello my Tel. is 322-0592";
        auto result = r.match(target, 0);

        REQUIRE(result.size() == 3);
        REQUIRE(result[0].substr(target) == "322-0592");
        REQUIRE(result[1].substr(target) == "322");
        REQUIRE(result[2].substr(target) == "0592");
    }

    SECTION("regex can retrieve the source") {
        Regex r("\\d{3}-\\d{4}");
        
        REQUIRE(r.source() == "\\d{3}-\\d{4}");
    }

    SECTION("invalid regex will throw") {
        REQUIRE_THROWS_AS(Regex r("[}"), InvalidRegexException);
    }

    SECTION("default contructor create a empty regex") {
        Regex regex;

        REQUIRE(regex.empty());
    }

    SECTION("empty regex match will throw") {
        Regex regex;

        REQUIRE_THROWS_AS(regex.match("hello world", 0), InvalidRegexException);
    }

    SECTION("\"\" as regex will match anything") {
        Regex regex("");
        auto result = regex.match("abcdefg", 0);

        REQUIRE( result == Match::MATCHED );
        REQUIRE( result[0] == Range(0,0) );
    }

    SECTION("regex can search from a position") {
        Regex r("abc");

        auto result = r.match("abcdefg", 0);
        REQUIRE(result == Match::MATCHED);

        auto result2 = r.match("abcdefg", 2);
        REQUIRE(result2 == Match::NOT_MATCHED);
    }

    SECTION("\\G means matching start position") {
        Regex r("\\Gabc");

        auto result1 = r.match("abcdef", 0);
        REQUIRE(result1 == Match::MATCHED);

        auto result2 = r.match(" abcdef", 0);
        REQUIRE(result2 == Match::NOT_MATCHED);

        auto result3 = r.match(" abcdef", 1);
        REQUIRE(result3 == Match::MATCHED);

        Regex r2("(?<=123)\\Gabc");
        auto result4 = r2.match("123abc", 3);
        REQUIRE(result4 == Match::MATCHED);
    }

    SECTION("\\G means last match end, perl compatible") {
        Regex r("\\Gabc");

        auto result1 = r.match("bstabcdef", 3, 3);
        REQUIRE(result1 == Match::MATCHED);

        auto result2 = r.match("bstabcdef", 3, 0);
        REQUIRE(result2 == Match::NOT_MATCHED);

        auto result3 = r.match("bstabcdef", 0, 3);
        REQUIRE(result2 == Match::NOT_MATCHED);


        Regex r2("(?<=bst)\\Gabc");
        auto result4 = r2.match("bstabcdef", 3, 3);
        REQUIRE(result4 == Match::MATCHED);

        auto result5 = r2.match("bstabcdef", 0, 3);
        REQUIRE(result5 == Match::NOT_MATCHED);

        Regex r3("\\Gabc");
        auto result6 = r3.match("bstabcdef", 0, 3);
        REQUIRE(result6 == Match::NOT_MATCHED);
    }
}


TEST_CASE("EndPatternRegex Test") {
    SECTION("EndPatternRegex can do everything Regex can do") {
        EndPatternRegex r("^(\\d{3})-(\\d{4})");
        string str = "123-3456";

        auto match = r.match(Match::make_dummy(0,0), str, 0);

        REQUIRE( match.size() == 3 );
    }

    SECTION("EndPatternRegex will throw when invalid regex is passed") {
        REQUIRE_THROWS_AS(EndPatternRegex r("[}"), InvalidRegexException);
    }

    SECTION("EndPatternRegex will recgonize regex that has back references") {
        EndPatternRegex r("</\\1>");
        REQUIRE( r.has_backref() );

        EndPatternRegex r1("^(\\d{3})-(\\d{4})");
        REQUIRE_FALSE( r1.has_backref() );

        EndPatternRegex r2("^(\\d{3})-(\\\\\\\\1{4})");
        REQUIRE_FALSE( r2.has_backref() );

        EndPatternRegex r3("^(\\d{3})-(\\\\\\1{4})");
        REQUIRE( r3.has_backref() );
    }

    SECTION("can capture external back reference") {
        Regex r1("<(\\w+)>");
        EndPatternRegex r2("</\\1>");
        string str = "<div>hello</div>";

        REQUIRE( r2.has_backref());

        auto begin = r1.match(str, 0);
        auto end = r2.match(begin, str, 0);

        REQUIRE( begin.size() == 2 );
        REQUIRE( end == Match::MATCHED );
        REQUIRE( end.size() == 1 );
        REQUIRE( end[0].substr(str) == "</div>" );
    }

    SECTION("can capture many back reference") {
        Regex r1("(a)(b)(c)(d)(e)(f)(g)(h)(i)(j)(k)(l)(\\d*)");
        EndPatternRegex r2("\\13haha");
        string src = "abcdefghijkl12345hahaend";

        REQUIRE( r2.has_backref() );

        auto begin = r1.match(src, 0);
        REQUIRE( begin.size() == 14 );

        auto end = r2.match(begin, src, 0);
        REQUIRE( end == Match::MATCHED );
        REQUIRE( end.size() == 1 );
        REQUIRE( end[0].substr(src) == "12345haha" );
    }

    SECTION("will throw when extelnal back reference out of range") {
        Regex r1("\\[");
        EndPatternRegex r2("\\1hey");
        string str = "[thisis[hey";

        REQUIRE( r2.has_backref() );

        auto begin = r1.match(str, 0);
        REQUIRE( begin == Match::MATCHED );

        REQUIRE_THROWS_AS(r2.match(begin, str, 0), InvalidRegexException);
    }

    SECTION("the external back reference will be escaped properly") {
        Regex r1("\\[");
        EndPatternRegex r2("\\0hey");
        string str = "[thisis[hey";

        REQUIRE( r2.has_backref() );

        auto begin = r1.match(str, 0);
        REQUIRE( begin == Match::MATCHED );

        auto end = r2.match(begin, str, 0);
        REQUIRE( end == Match::MATCHED );
        REQUIRE( end.size() == 1 );
        REQUIRE( end[0].substr(str) == "[hey" );
    }

    SECTION("the expanded end rule will only escape the inserted string") {
        Regex r1("\\[");
        EndPatternRegex r2("\\[\\0hey");
        string str = "[thisis[[hey";

        REQUIRE( r2.has_backref() );

        auto begin = r1.match(str, 0);
        REQUIRE( begin == Match::MATCHED );

        auto end = r2.match(begin, str, 0);
        REQUIRE( end == Match::MATCHED );
        REQUIRE( end.size() == 1 );
        REQUIRE( end[0].substr(str) == "[[hey" );
    }
}
