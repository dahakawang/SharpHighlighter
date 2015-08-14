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
        REQUIRE( tokens[0].second.name() == "source.hello" );

        REQUIRE( tokens[1].first.substr(source) == "hello" );
        REQUIRE( tokens[1].second.name() == "source.hello prefix.hello" );

        REQUIRE( tokens[2].first.substr(source) == "world!" );
        REQUIRE( tokens[2].second.name() == "source.hello suffix.hello" );

        REQUIRE( tokens[3].first.substr(source) == "!" );
        REQUIRE( tokens[3].second.name() == "source.hello suffix.hello emphasis.hello" );
    }

    SECTION("return a single token when matches a single pattern with no capture groups") {
        string data = load_string("fixture/coffee-script.json");
        string source = "return";
        Grammar g = loader.load(data);
        auto tokens = tokenizer.tokenize(g, source); 

        REQUIRE( tokens.size() == 2 );
        REQUIRE( tokens[1].first.substr(source) == "return" );
        REQUIRE( tokens[1].second.name() == "source.coffee keyword.control.coffee" );
    }

    SECTION("return several tokens when matches a single pattern with capture gorups") {
        string data = load_string("fixture/coffee-script.json");
        string source = "new foo.bar.Baz";
        Grammar g = loader.load(data);
        auto tokens = tokenizer.tokenize(g, source); 

        REQUIRE( tokens.size() == 4 );
        
        REQUIRE( tokens[1].first.substr(source) == source );
        REQUIRE( tokens[1].second.name() == "source.coffee meta.class.instance.constructor" );
        
        REQUIRE( tokens[2].first.substr(source) == "new" );
        REQUIRE( tokens[2].second.name() == "source.coffee meta.class.instance.constructor keyword.operator.new.coffee" );

        REQUIRE( tokens[3].first.substr(source) == "foo.bar.Baz" );
        REQUIRE( tokens[3].second.name() == "source.coffee meta.class.instance.constructor entity.name.type.instance.coffee" );
    }

    SECTION("return grammar top level token when no match at all") {
        string data = load_string("fixture/text.json");
        string source = "   ";
        Grammar g = loader.load(data);
        auto tokens = tokenizer.tokenize(g, source); 
        
        REQUIRE( tokens.size() == 1 );

        REQUIRE( tokens[0].first.substr(source) == source );
        REQUIRE( tokens[0].second.name() == "text.plain" );
    }

    SECTION("the enclosing scope will cover the sub-scopes") {
        string data = load_string("fixture/coffee-script.json");
        string source = " return new foo.bar.Baz ";
        Grammar g = loader.load(data);
        auto tokens = tokenizer.tokenize(g, source); 

        REQUIRE( tokens.size() == 5 );
        REQUIRE (tokens[0].first.substr(source) == source );
        REQUIRE (tokens[0].second.name() == "source.coffee" );

        REQUIRE (tokens[1].first.substr(source) == "return" );
        REQUIRE (tokens[1].second.name() == "source.coffee keyword.control.coffee" );

        REQUIRE (tokens[2].first.substr(source) == "new foo.bar.Baz" );
        REQUIRE (tokens[2].second.name() == "source.coffee meta.class.instance.constructor" );

        REQUIRE (tokens[3].first.substr(source) == "new" );
        REQUIRE (tokens[3].second.name() == "source.coffee meta.class.instance.constructor keyword.operator.new.coffee" );

        REQUIRE (tokens[4].first.substr(source) == "foo.bar.Baz" );
        REQUIRE (tokens[4].second.name() == "source.coffee meta.class.instance.constructor entity.name.type.instance.coffee" );
    }

    SECTION("only return matched capture groups when match rule has an optional captre groups") {
        string data = load_string("fixture/coffee-script.json");
        string source = "class Foo";
        Grammar g = loader.load(data);
        auto tokens = tokenizer.tokenize(g, source); 

        REQUIRE( tokens.size() == 4 );

        REQUIRE( tokens[0].first.substr(source) == source ); 
        REQUIRE (tokens[0].second.name() == "source.coffee" );

        REQUIRE( tokens[1].first.substr(source) == source ); 
        REQUIRE (tokens[1].second.name() == "source.coffee meta.class.coffee" );

        REQUIRE( tokens[2].first.substr(source) == "class" ); 
        REQUIRE (tokens[2].second.name() == "source.coffee meta.class.coffee storage.type.class.coffee" );

        REQUIRE( tokens[3].first.substr(source) == "Foo" ); 
        REQUIRE (tokens[3].second.name() == "source.coffee meta.class.coffee entity.name.type.class.coffee" );
    }
}
