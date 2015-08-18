#include "catch.hpp"
#include "util.h"
#include <shl_exception.h>
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

    SECTION("it will loop infinitely") {
        string data = load_string("fixture/text.json");
        string source = "hoo";
        Grammar g = loader.load(data);
        Tokenizer tokenizer(Tokenizer::OPTION_TOLERATE_ERROR);

        REQUIRE_NOTHROW( tokenizer.tokenize(g, source) );
    }

    SECTION("will throw when scope is not properly closed (i.e. source code is malformed)") {
        string data = load_string("fixture/c.json");
        string source = "int main() {";
        Grammar g = loader.load(data);
        
        REQUIRE_THROWS_AS(tokenizer.tokenize(g, source), InvalidSourceException);
    }

    SECTION("will not throw when scope is not properly closed, if OPTION_TOLERATE_ERROR is specified") {
        string data = load_string("fixture/c.json");
        string source = "int main() {";
        Grammar g = loader.load(data);
        Tokenizer tokenizer(Tokenizer::OPTION_TOLERATE_ERROR);
        
        REQUIRE_NOTHROW( tokenizer.tokenize(g, source) );
    }

    SECTION("the enclosing scope will cover the sub-scopes") {
        string data = load_string("fixture/coffee-script.json");
        string source = " return new foo.bar.Baz ";
        Grammar g = loader.load(data);
        auto tokens = tokenizer.tokenize(g, source); 

        REQUIRE( tokens.size() == 5 );
        REQUIRE( tokens[0].first.substr(source) == source );
        REQUIRE( tokens[0].second.name() == "source.coffee" );

        REQUIRE( tokens[1].first.substr(source) == "return" );
        REQUIRE( tokens[1].second.name() == "source.coffee keyword.control.coffee" );

        REQUIRE( tokens[2].first.substr(source) == "new foo.bar.Baz" );
        REQUIRE( tokens[2].second.name() == "source.coffee meta.class.instance.constructor" );

        REQUIRE( tokens[3].first.substr(source) == "new" );
        REQUIRE( tokens[3].second.name() == "source.coffee meta.class.instance.constructor keyword.operator.new.coffee" );

        REQUIRE( tokens[4].first.substr(source) == "foo.bar.Baz" );
        REQUIRE( tokens[4].second.name() == "source.coffee meta.class.instance.constructor entity.name.type.instance.coffee" );
    }

    SECTION("only return matched capture groups when match rule has an optional captre groups") {
        string data = load_string("fixture/coffee-script.json");
        string source = "class Foo";
        Grammar g = loader.load(data);
        auto tokens = tokenizer.tokenize(g, source); 

        REQUIRE( tokens.size() == 4 );

        REQUIRE( tokens[0].first.substr(source) == source ); 
        REQUIRE( tokens[0].second.name() == "source.coffee" );

        REQUIRE( tokens[1].first.substr(source) == source ); 
        REQUIRE( tokens[1].second.name() == "source.coffee meta.class.coffee" );

        REQUIRE( tokens[2].first.substr(source) == "class" ); 
        REQUIRE( tokens[2].second.name() == "source.coffee meta.class.coffee storage.type.class.coffee" );

        REQUIRE( tokens[3].first.substr(source) == "Foo" ); 
        REQUIRE( tokens[3].second.name() == "source.coffee meta.class.coffee entity.name.type.class.coffee" );
    }

    /* The tokens are sorted in the way similar to the in-order traversal of the AST
     * This holds true even for begin_captures/end_capture/captures rules, because 
     * capture group themselves have hierarchy. Although capture rules are listed in 
     * any order, since we simply apply scope to the captured group in ascending order(0,1...N),
     * we are actually apply in the same way of the in-order traversal.
     *
     * e.g. ((()())()), the capture group number are corresponding to below tree
     *             1
     *           /   \
     *          2     5
     *         / \
     *        3   4
     */
    SECTION("when encounter nested capture, ensure parent capture group appears ahead of child") {
        string data = load_string("fixture/coffee-script.json");
        string source = "  destroy: ->";
        Grammar g = loader.load(data);
        auto tokens = tokenizer.tokenize(g, source); 

        REQUIRE( tokens.size() == 6 );

        REQUIRE( tokens[0].first.substr(source) == source ); 
        REQUIRE( tokens[0].second.name() == "source.coffee" );

        REQUIRE( tokens[1].first.substr(source) == "destroy" ); 
        REQUIRE( tokens[1].second.name() == "source.coffee meta.function.coffee" );

        REQUIRE( tokens[2].first.substr(source) == "destroy" ); 
        REQUIRE( tokens[2].second.name() == "source.coffee meta.function.coffee entity.name.function.coffee" );

        REQUIRE( tokens[3].first.substr(source) == "y" ); 
        REQUIRE( tokens[3].second.name() == "source.coffee meta.function.coffee entity.name.function.coffee entity.name.function.coffee" );

        REQUIRE( tokens[4].first.substr(source) == ":" ); 
        REQUIRE( tokens[4].second.name() == "source.coffee keyword.operator.coffee" );

        REQUIRE( tokens[5].first.substr(source) == "->" ); 
        REQUIRE( tokens[5].second.name() == "source.coffee storage.type.function.coffee" );
    }

    SECTION("when capture beyond the matched range (i.e. capture in a look ahead/behind group)") {
        string data = load_string("fixture/coffee-script.json");
        string source = "  destroy: ->";
        Grammar g = loader.load(data);
        auto tokens = tokenizer.tokenize(g, source); 

        REQUIRE( tokens.size() == 6 );

        // the scope "source.coffee meta.function.coffee storage.type.function.coffee" captures
        // "->", which is beyond the matching range. so ignore it.
        REQUIRE_FALSE( tokens[4].first.substr(source) == "->" ); 
        REQUIRE_FALSE( tokens[4].second.name() == "source.coffee meta.function.coffee storage.type.function.coffee" );
    }

    SECTION("the enclosed/nested capture group will has additional ScopeNames") {
        string data = load_string("fixture/coffee-script.json");
        string source = "  destroy: ->";
        Grammar g = loader.load(data);
        auto tokens = tokenizer.tokenize(g, source); 

        REQUIRE( tokens.size() == 6 );

        REQUIRE( tokens[2].first.substr(source) == "destroy" ); 
        REQUIRE( tokens[2].second.name() == "source.coffee meta.function.coffee entity.name.function.coffee" );

        REQUIRE( tokens[3].first.substr(source) == "y" ); 
        REQUIRE( tokens[3].second.name() == "source.coffee meta.function.coffee entity.name.function.coffee entity.name.function.coffee" );

    }

    SECTION("the include rule wil do its work") {
        string data = load_string("fixture/coffee-script.json");
        string source = "1233456";
        Grammar g = loader.load(data);
        auto tokens = tokenizer.tokenize(g, source); 

        REQUIRE( tokens.size() == 2 );
        
        REQUIRE( tokens[0].first.substr(source) == source ); 
        REQUIRE( tokens[0].second.name() == "source.coffee" );

        REQUIRE( tokens[1].first.substr(source) == "1233456" ); 
        REQUIRE( tokens[1].second.name() == "source.coffee constant.numeric.coffee" );
    }

    SECTION("can handle interpolated string") {
        string data = load_string("fixture/coffee-script.json");
        string source = "\"the value is #{@x} my friend\"";
        Grammar g = loader.load(data);
        auto tokens = tokenizer.tokenize(g, source); 

        REQUIRE( tokens.size() == 8 );

        REQUIRE( tokens[0].first.substr(source) == source ); 
        REQUIRE( tokens[0].second.name() == "source.coffee" );
        
        REQUIRE( tokens[1].first.substr(source) == source ); 
        REQUIRE( tokens[1].second.name() == "source.coffee string.quoted.double.coffee" );

        REQUIRE( tokens[2].first.substr(source) == "\"" ); 
        REQUIRE( tokens[2].second.name() == "source.coffee string.quoted.double.coffee punctuation.definition.string.begin.coffee" );

        REQUIRE( tokens[3].first.substr(source) == "#{@x}" ); 
        REQUIRE( tokens[3].second.name() == "source.coffee string.quoted.double.coffee source.coffee.embedded.source" );

        REQUIRE( tokens[4].first.substr(source) == "#{" ); 
        REQUIRE( tokens[4].second.name() == "source.coffee string.quoted.double.coffee source.coffee.embedded.source punctuation.section.embedded.coffee" );

        REQUIRE( tokens[5].first.substr(source) == "@x" ); 
        REQUIRE( tokens[5].second.name() == "source.coffee string.quoted.double.coffee source.coffee.embedded.source variable.other.readwrite.instance.coffee" );

        REQUIRE( tokens[6].first.substr(source) == "}" ); 
        REQUIRE( tokens[6].second.name() == "source.coffee string.quoted.double.coffee source.coffee.embedded.source punctuation.section.embedded.coffee" );

        REQUIRE( tokens[7].first.substr(source) == "\"" ); 
        REQUIRE( tokens[7].second.name() == "source.coffee string.quoted.double.coffee punctuation.definition.string.end.coffee" );
    }

    SECTION("can handle nested e interpolated string") {
        string data = load_string("fixture/coffee-script.json");
        string source = "\"#{\"#{@x}\"}\"";
        Grammar g = loader.load(data);
        auto tokens = tokenizer.tokenize(g, source); 

        REQUIRE( tokens.size() == 14 );

        REQUIRE( tokens[0].first.substr(source) == source ); 
        REQUIRE( tokens[0].second.name() == "source.coffee" );

        REQUIRE( tokens[1].first.substr(source) == source ); 
        REQUIRE( tokens[1].second.name() == "source.coffee string.quoted.double.coffee" );

        REQUIRE( tokens[2].first.substr(source) == "\"" ); 
        REQUIRE( tokens[2].second.name() == "source.coffee string.quoted.double.coffee punctuation.definition.string.begin.coffee" );

        REQUIRE( tokens[3].first.substr(source) == "#{\"#{@x}\"}" ); 
        REQUIRE( tokens[3].second.name() == "source.coffee string.quoted.double.coffee source.coffee.embedded.source" );

        REQUIRE( tokens[4].first.substr(source) == "#{" ); 
        REQUIRE( tokens[4].second.name() == "source.coffee string.quoted.double.coffee source.coffee.embedded.source punctuation.section.embedded.coffee" );

        REQUIRE( tokens[5].first.substr(source) == "\"#{@x}\"" ); 
        REQUIRE( tokens[5].second.name() == "source.coffee string.quoted.double.coffee source.coffee.embedded.source string.quoted.double.coffee" );

        REQUIRE( tokens[6].first.substr(source) == "\"" ); 
        REQUIRE( tokens[6].second.name() == "source.coffee string.quoted.double.coffee source.coffee.embedded.source string.quoted.double.coffee punctuation.definition.string.begin.coffee" );

        REQUIRE( tokens[7].first.substr(source) == "#{@x}" ); 
        REQUIRE( tokens[7].second.name() == "source.coffee string.quoted.double.coffee source.coffee.embedded.source string.quoted.double.coffee source.coffee.embedded.source" );

        REQUIRE( tokens[8].first.substr(source) == "#{" ); 
        REQUIRE( tokens[8].second.name() == "source.coffee string.quoted.double.coffee source.coffee.embedded.source string.quoted.double.coffee source.coffee.embedded.source punctuation.section.embedded.coffee" );

        REQUIRE( tokens[9].first.substr(source) == "@x" ); 
        REQUIRE( tokens[9].second.name() == "source.coffee string.quoted.double.coffee source.coffee.embedded.source string.quoted.double.coffee source.coffee.embedded.source variable.other.readwrite.instance.coffee" );

        REQUIRE( tokens[10].first.substr(source) == "}" ); 
        REQUIRE( tokens[10].second.name() == "source.coffee string.quoted.double.coffee source.coffee.embedded.source string.quoted.double.coffee source.coffee.embedded.source punctuation.section.embedded.coffee" );

        REQUIRE( tokens[11].first.substr(source) == "\"" ); 
        REQUIRE( tokens[11].second.name() == "source.coffee string.quoted.double.coffee source.coffee.embedded.source string.quoted.double.coffee punctuation.definition.string.end.coffee" );

        REQUIRE( tokens[12].first.substr(source) == "}" ); 
        REQUIRE( tokens[12].second.name() == "source.coffee string.quoted.double.coffee source.coffee.embedded.source punctuation.section.embedded.coffee" );

        REQUIRE( tokens[13].first.substr(source) == "\"" ); 
        REQUIRE( tokens[13].second.name() == "source.coffee string.quoted.double.coffee punctuation.definition.string.end.coffee" );
    }

    SECTION("return top level scope when empty source") {
        string data = load_string("fixture/coffee-script.json");
        string source = "";
        Grammar g = loader.load(data);
        auto tokens = tokenizer.tokenize(g, source); 

        REQUIRE( tokens.size() == 1 );
    }

    SECTION("contentName rule will return token name after its content") {
        string data = load_string("fixture/content-name.json");
        string source = "#if\ntest\n#endif";
        Grammar g = loader.load(data);
        Tokenizer tokenizer(Tokenizer::OPTION_TOLERATE_ERROR);
        auto tokens = tokenizer.tokenize(g, source); 

        REQUIRE( tokens.size() == 3 );


        REQUIRE( tokens[0].first.substr(source) == source ); 
        REQUIRE( tokens[0].second.name() == "source.test" );

        REQUIRE( tokens[1].first.substr(source) == source ); 
        REQUIRE( tokens[1].second.name() == "source.test pre" );

        REQUIRE( tokens[2].first.substr(source) == "\ntest\n" ); 
        REQUIRE( tokens[2].second.name() == "source.test pre nested" );
    }
    // TODO test $base $self
}
