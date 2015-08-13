#include "catch.hpp"
#include "util.h"
#include <tokenizer.h>
#include <grammar_loader.h>

using namespace shl;

TEST_CASE("Tokenizer Tests") {
    Tokenizer tokenizer;
    GrammarLoader loader;

    SECTION("can load grammar and tokenizer string") {
        string data = load_string("fixture/hello.json");
        string source = "hello world!";
        Grammar g = loader.load(data);
        auto tokens = tokenizer.tokenize(g, source); 

        REQUIRE( tokens.size() == 4 );
        REQUIRE( tokens[0].first.substr(source) == source );
        REQUIRE( tokens[0].second.name() == "source.hello");

        REQUIRE( tokens[1].first.substr(source) == "hello" );
        REQUIRE( tokens[1].second.name() == "source.hello prefix.hello");

        REQUIRE( tokens[2].first.substr(source) == "world!" );
        REQUIRE( tokens[2].second.name() == "source.hello suffix.hello");

        REQUIRE( tokens[3].first.substr(source) == "!" );
        REQUIRE( tokens[3].second.name() == "source.hello suffix.hello emphasis.hello");
    }
}
