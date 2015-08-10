//#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <regex.h>
#include <iostream>

using namespace shl;
using namespace std;

TEST_CASE("Regex can find matchs") {
    SECTION("find a simple match") {
        Regex r("\\d{3}-\\d{4}");
        auto result = r.match("hello my Tel. is 322-0592", 0);
        if (result == Match::NOT_MATCHED) {
            cout << "not matched" << endl;
        }else {
            cout << "matched" << endl;
        }
    }
}
