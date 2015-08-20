#include "catch.hpp"
#include "util.h"
#include <shl_exception.h>
#include <tokenizer.h>
#include <grammar_compiler.h>
#include <grammar_registry.h>

using namespace shl;

TEST_CASE("Tokenizer Tests") {
    Tokenizer tokenizer;
    GrammarCompiler compiler;
    shared_ptr<TestGrammarSource> grammar_source(new TestGrammarSource({
            "source.ruby", "fixture/ruby.json",
            "text.html.basic", "fixture/html.json"}));
    GrammarRegistry registry(grammar_source);

    SECTION("can load grammar and tokenizer string") {
        string data = load_string("fixture/hello.json");
        string source = "hello world!";
        Grammar g = compiler.compile(data);
        compiler.resolve_include(g, nullptr);
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
        Grammar g = compiler.compile(data);
        compiler.resolve_include(g, nullptr);
        auto tokens = tokenizer.tokenize(g, source); 

        REQUIRE( tokens.size() == 2 );
        REQUIRE( tokens[1].first.substr(source) == "return" );
        REQUIRE( tokens[1].second.name() == "source.coffee keyword.control.coffee" );
    }

    SECTION("return several tokens when matches a single pattern with capture gorups") {
        string data = load_string("fixture/coffee-script.json");
        string source = "new foo.bar.Baz";
        Grammar g = compiler.compile(data);
        compiler.resolve_include(g, nullptr);
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
        Grammar g = compiler.compile(data);
        compiler.resolve_include(g, nullptr);
        auto tokens = tokenizer.tokenize(g, source); 
        
        REQUIRE( tokens.size() == 1 );

        REQUIRE( tokens[0].first.substr(source) == source );
        REQUIRE( tokens[0].second.name() == "text.plain" );
    }

    SECTION("it will not loop infinitely") {
        string data = load_string("fixture/text.json");
        string source = "hoo";
        Grammar g = compiler.compile(data);
        compiler.resolve_include(g, nullptr);
        Tokenizer tokenizer(Tokenizer::OPTION_TOLERATE_ERROR);

        REQUIRE_NOTHROW( tokenizer.tokenize(g, source) );
    }

    SECTION("will throw when scope is not properly closed (i.e. source code is malformed)") {
        string data = load_string("fixture/c.json");
        string source = "int main() {";
        Grammar g = compiler.compile(data);
        compiler.resolve_include(g, nullptr);
        
        REQUIRE_THROWS_AS(tokenizer.tokenize(g, source), InvalidSourceException);
    }

    SECTION("will not throw when scope is not properly closed, if OPTION_TOLERATE_ERROR is specified") {
        string data = load_string("fixture/c.json");
        string source = "int main() {";
        Grammar g = compiler.compile(data);
        compiler.resolve_include(g, nullptr);
        Tokenizer tokenizer(Tokenizer::OPTION_TOLERATE_ERROR);
        
        REQUIRE_NOTHROW( tokenizer.tokenize(g, source) );
    }

    SECTION("the enclosing scope will cover the sub-scopes") {
        string data = load_string("fixture/coffee-script.json");
        string source = " return new foo.bar.Baz ";
        Grammar g = compiler.compile(data);
        compiler.resolve_include(g, nullptr);
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
        Grammar g = compiler.compile(data);
        compiler.resolve_include(g, nullptr);
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
        Grammar g = compiler.compile(data);
        compiler.resolve_include(g, nullptr);
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
        Grammar g = compiler.compile(data);
        compiler.resolve_include(g, nullptr);
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
        Grammar g = compiler.compile(data);
        compiler.resolve_include(g, nullptr);
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
        Grammar g = compiler.compile(data);
        compiler.resolve_include(g, nullptr);
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
        Grammar g = compiler.compile(data);
        compiler.resolve_include(g, nullptr);
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
        REQUIRE( tokens[5].second.name() == "source.coffee string.quoted.double.coffee source.coffee.embedded.source source.coffee variable.other.readwrite.instance.coffee" );

        REQUIRE( tokens[6].first.substr(source) == "}" ); 
        REQUIRE( tokens[6].second.name() == "source.coffee string.quoted.double.coffee source.coffee.embedded.source punctuation.section.embedded.coffee" );

        REQUIRE( tokens[7].first.substr(source) == "\"" ); 
        REQUIRE( tokens[7].second.name() == "source.coffee string.quoted.double.coffee punctuation.definition.string.end.coffee" );
    }

    SECTION("can handle nested interpolated string") {
        string data = load_string("fixture/coffee-script.json");
        string source = "\"#{\"#{@x}\"}\"";
        Grammar g = compiler.compile(data);
        compiler.resolve_include(g, nullptr);
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
        REQUIRE( tokens[5].second.name() == "source.coffee string.quoted.double.coffee source.coffee.embedded.source source.coffee string.quoted.double.coffee" );

        REQUIRE( tokens[6].first.substr(source) == "\"" ); 
        REQUIRE( tokens[6].second.name() == "source.coffee string.quoted.double.coffee source.coffee.embedded.source source.coffee string.quoted.double.coffee punctuation.definition.string.begin.coffee" );

        REQUIRE( tokens[7].first.substr(source) == "#{@x}" ); 
        REQUIRE( tokens[7].second.name() == "source.coffee string.quoted.double.coffee source.coffee.embedded.source source.coffee string.quoted.double.coffee source.coffee.embedded.source" );

        REQUIRE( tokens[8].first.substr(source) == "#{" ); 
        REQUIRE( tokens[8].second.name() == "source.coffee string.quoted.double.coffee source.coffee.embedded.source source.coffee string.quoted.double.coffee source.coffee.embedded.source punctuation.section.embedded.coffee" );

        REQUIRE( tokens[9].first.substr(source) == "@x" ); 
        REQUIRE( tokens[9].second.name() == "source.coffee string.quoted.double.coffee source.coffee.embedded.source source.coffee string.quoted.double.coffee source.coffee.embedded.source source.coffee variable.other.readwrite.instance.coffee" );

        REQUIRE( tokens[10].first.substr(source) == "}" ); 
        REQUIRE( tokens[10].second.name() == "source.coffee string.quoted.double.coffee source.coffee.embedded.source source.coffee string.quoted.double.coffee source.coffee.embedded.source punctuation.section.embedded.coffee" );

        REQUIRE( tokens[11].first.substr(source) == "\"" ); 
        REQUIRE( tokens[11].second.name() == "source.coffee string.quoted.double.coffee source.coffee.embedded.source source.coffee string.quoted.double.coffee punctuation.definition.string.end.coffee" );

        REQUIRE( tokens[12].first.substr(source) == "}" ); 
        REQUIRE( tokens[12].second.name() == "source.coffee string.quoted.double.coffee source.coffee.embedded.source punctuation.section.embedded.coffee" );

        REQUIRE( tokens[13].first.substr(source) == "\"" ); 
        REQUIRE( tokens[13].second.name() == "source.coffee string.quoted.double.coffee punctuation.definition.string.end.coffee" );
    }

    SECTION("return top level scope when empty source") {
        string data = load_string("fixture/coffee-script.json");
        string source = "";
        Grammar g = compiler.compile(data);
        compiler.resolve_include(g, nullptr);
        auto tokens = tokenizer.tokenize(g, source); 

        REQUIRE( tokens.size() == 1 );
    }

    SECTION("contentName rule will return token name after its content") {
        string data = load_string("fixture/content-name.json");
        string source = "#if\ntest\n#endif";
        Grammar g = compiler.compile(data);
        compiler.resolve_include(g, nullptr);
        Tokenizer tokenizer(Tokenizer::OPTION_TOLERATE_ERROR);
        auto tokens = tokenizer.tokenize(g, source); 

        REQUIRE( tokens.size() == 3 );

        REQUIRE( tokens[0].first.substr(source) == source ); 
        REQUIRE( tokens[0].second.name() == "source.test" );

        REQUIRE( tokens[1].first.substr(source) == source ); 
        REQUIRE( tokens[1].second.name() == "source.test pre" );

        REQUIRE( tokens[2].first.substr(source) == "\ntest\n" ); 
        REQUIRE( tokens[2].second.name() == "source.test pre nested" );


        source = "testfin";
        tokens = tokenizer.tokenize(g, source);

        REQUIRE( tokens.size() == 3 );

        REQUIRE( tokens[0].first.substr(source) == source ); 
        REQUIRE( tokens[0].second.name() == "source.test" );

        REQUIRE( tokens[1].first.substr(source) == source ); 
        REQUIRE( tokens[1].second.name() == "source.test all" );

        REQUIRE( tokens[2].first.substr(source) == "test" ); 
        REQUIRE( tokens[2].second.name() == "source.test all middle" );

        source = "  test fin";
        tokens = tokenizer.tokenize(g, source);

        REQUIRE( tokens.size() == 3 );

        REQUIRE( tokens[0].first.substr(source) == source ); 
        REQUIRE( tokens[0].second.name() == "source.test" );

        REQUIRE( tokens[1].first.substr(source) == source ); 
        REQUIRE( tokens[1].second.name() == "source.test all" );

        REQUIRE( tokens[2].first.substr(source) == "test " ); 
        REQUIRE( tokens[2].second.name() == "source.test all middle" );
    }

    SECTION("scope will be ignored if the pattern contains no name or contentName") {
        string source = "%w|oh \\look|";
        Grammar g = registry.get_grammar("source.ruby");
        auto tokens = tokenizer.tokenize(g, source); 

        REQUIRE( tokens.size() == 4 );


        REQUIRE( tokens[0].first.substr(source) == source ); 
        REQUIRE( tokens[0].second.name() == "source.ruby" );

        REQUIRE( tokens[1].first.substr(source) == source ); 
        REQUIRE( tokens[1].second.name() == "source.ruby string.quoted.other.literal.lower.ruby" );

        REQUIRE( tokens[2].first.substr(source) == "%w|" ); 
        REQUIRE( tokens[2].second.name() == "source.ruby string.quoted.other.literal.lower.ruby punctuation.definition.string.begin.ruby" );

        REQUIRE( tokens[3].first.substr(source) == "|" ); 
        REQUIRE( tokens[3].second.name() == "source.ruby string.quoted.other.literal.lower.ruby punctuation.definition.string.end.ruby" );
    }

    SECTION("the beginCapture, endCapture and chid scope will capture lexeme") {
        string data = load_string("fixture/coffee-script.json");
        string source = "'''content of heredoc'''";
        Grammar g = compiler.compile(data);
        compiler.resolve_include(g, nullptr);
        auto tokens = tokenizer.tokenize(g, source); 

        REQUIRE( tokens.size() == 4 );

        REQUIRE( tokens[0].first.substr(source) == source ); 
        REQUIRE( tokens[0].second.name() == "source.coffee" );

        REQUIRE( tokens[1].first.substr(source) == source ); 
        REQUIRE( tokens[1].second.name() == "source.coffee string.quoted.heredoc.coffee" );

        REQUIRE( tokens[2].first.substr(source) == "'''" ); 
        REQUIRE( tokens[2].second.name() == "source.coffee string.quoted.heredoc.coffee punctuation.definition.string.begin.coffee" );

        REQUIRE( tokens[3].first.substr(source) == "'''" ); 
        REQUIRE( tokens[3].second.name() == "source.coffee string.quoted.heredoc.coffee punctuation.definition.string.end.coffee" );
    }

    SECTION("mutli-line desen't affect us") {
        string data = load_string("fixture/coffee-script.json");
        string source = "'''content\n of\n heredoc'''";
        Grammar g = compiler.compile(data);
        compiler.resolve_include(g, nullptr);
        auto tokens = tokenizer.tokenize(g, source); 

        REQUIRE( tokens.size() == 4 );

        REQUIRE( tokens[0].first.substr(source) == source ); 
        REQUIRE( tokens[0].second.name() == "source.coffee" );

        REQUIRE( tokens[1].first.substr(source) == source ); 
        REQUIRE( tokens[1].second.name() == "source.coffee string.quoted.heredoc.coffee" );

        REQUIRE( tokens[2].first.substr(source) == "'''" ); 
        REQUIRE( tokens[2].second.name() == "source.coffee string.quoted.heredoc.coffee punctuation.definition.string.begin.coffee" );

        REQUIRE( tokens[3].first.substr(source) == "'''" ); 
        REQUIRE( tokens[3].second.name() == "source.coffee string.quoted.heredoc.coffee punctuation.definition.string.end.coffee" );
    }

    SECTION("the sub-patterns will capture scopes") {
        string data = load_string("fixture/coffee-script.json");
        string source = "\"\"\"content \nof\\t h\neredoc\"\"\"";
        Grammar g = compiler.compile(data);
        compiler.resolve_include(g, nullptr);
        auto tokens = tokenizer.tokenize(g, source); 
        
        REQUIRE( tokens.size() == 5 );

        REQUIRE( tokens[0].first.substr(source) == source ); 
        REQUIRE( tokens[0].second.name() == "source.coffee" );

        REQUIRE( tokens[1].first.substr(source) == source ); 
        REQUIRE( tokens[1].second.name() == "source.coffee string.quoted.double.heredoc.coffee" );

        REQUIRE( tokens[2].first.substr(source) == "\"\"\"" ); 
        REQUIRE( tokens[2].second.name() == "source.coffee string.quoted.double.heredoc.coffee punctuation.definition.string.begin.coffee" );

        REQUIRE( tokens[3].first.substr(source) == "\\t" ); 
        REQUIRE( tokens[3].second.name() == "source.coffee string.quoted.double.heredoc.coffee constant.character.escape.coffee" );

        REQUIRE( tokens[4].first.substr(source) == "\"\"\"" ); 
        REQUIRE( tokens[4].second.name() == "source.coffee string.quoted.double.heredoc.coffee punctuation.definition.string.end.coffee" );
    }

    SECTION("applyEndPatternLast will make end rule less important when a begin pattern and end pattern all matches at the same position") {
        string data = load_string("fixture/apply-end-pattern-last.json");
        string source = 
        "last\n"
        "{ some }excentricSyntax }\n"
        "\n"
        "first\n"
        "{ some }excentricSyntax }\n"
        "\n";
        Grammar g = compiler.compile(data);
        compiler.resolve_include(g, nullptr);
        auto tokens = tokenizer.tokenize(g, source); 

        REQUIRE( tokens.size() == 6 );

        REQUIRE( tokens[0].first.substr(source) == source ); 
        REQUIRE( tokens[0].second.name() == "source.apply-end-pattern-last" );

        REQUIRE( tokens[1].first.substr(source) == 
        "last\n"
        "{ some }excentricSyntax }\n");
        REQUIRE( tokens[1].second.name() == "source.apply-end-pattern-last end-pattern-last-env" );

        REQUIRE( tokens[2].first.substr(source) == "{ some }excentricSyntax }" ); 
        REQUIRE( tokens[2].second.name() == "source.apply-end-pattern-last end-pattern-last-env scope" );

        REQUIRE( tokens[3].first.substr(source) == "}excentricSyntax" ); 
        REQUIRE( tokens[3].second.name() == "source.apply-end-pattern-last end-pattern-last-env scope excentric" );

        REQUIRE( tokens[4].first.substr(source) == 
        "first\n"
        "{ some }excentricSyntax }\n"); 
        REQUIRE( tokens[4].second.name() == "source.apply-end-pattern-last normal-env" );

        REQUIRE( tokens[5].first.substr(source) == "{ some }" ); 
        REQUIRE( tokens[5].second.name() == "source.apply-end-pattern-last normal-env scope" );
    }

    SECTION("the back reference in end pattern will refer to matches in begin pattern") {
        string source = "%w|oh|,";
        Grammar g = registry.get_grammar("source.ruby");
        auto tokens = tokenizer.tokenize(g, source); 

        REQUIRE( tokens.size() == 5 );

        REQUIRE( tokens[0].first.substr(source) == source ); 
        REQUIRE( tokens[0].second.name() == "source.ruby" );

        REQUIRE( tokens[1].first.substr(source) == "%w|oh|" ); 
        REQUIRE( tokens[1].second.name() == "source.ruby string.quoted.other.literal.lower.ruby" );

        REQUIRE( tokens[2].first.substr(source) == "%w|" ); 
        REQUIRE( tokens[2].second.name() == "source.ruby string.quoted.other.literal.lower.ruby punctuation.definition.string.begin.ruby" );

        REQUIRE( tokens[3].first.substr(source) == "|" ); 
        REQUIRE( tokens[3].second.name() == "source.ruby string.quoted.other.literal.lower.ruby punctuation.definition.string.end.ruby" );

        REQUIRE( tokens[4].first.substr(source) == "," ); 
        REQUIRE( tokens[4].second.name() == "source.ruby punctuation.separator.object.ruby" );
    }

    SECTION("end pattern that contains back reference can be different when the rule appears on the stack for several times") {
        string source = "%Q+matz had some #{%Q-crazy ideas-} for ruby syntax+ # damn.\n";
        Grammar g = registry.get_grammar("source.ruby");
        auto tokens = tokenizer.tokenize(g, source); 

        REQUIRE( tokens.size() == 14 );

        REQUIRE( tokens[0].first.substr(source) == source ); 
        REQUIRE( tokens[0].second.name() == "source.ruby" );

        REQUIRE( tokens[1].first.substr(source) == "%Q+matz had some #{%Q-crazy ideas-} for ruby syntax+"); 
        REQUIRE( tokens[1].second.name() == "source.ruby string.quoted.other.literal.upper.ruby" );

        REQUIRE( tokens[2].first.substr(source) == "%Q+"); 
        REQUIRE( tokens[2].second.name() == "source.ruby string.quoted.other.literal.upper.ruby punctuation.definition.string.begin.ruby" );

        REQUIRE( tokens[3].first.substr(source) == "#{%Q-crazy ideas-}"); 
        REQUIRE( tokens[3].second.name() == "source.ruby string.quoted.other.literal.upper.ruby meta.embedded.line.ruby" );

        REQUIRE( tokens[4].first.substr(source) == "#{"); 
        REQUIRE( tokens[4].second.name() == "source.ruby string.quoted.other.literal.upper.ruby meta.embedded.line.ruby punctuation.section.embedded.begin.ruby" );

        REQUIRE( tokens[5].first.substr(source) == "%Q-crazy ideas-"); 
        REQUIRE( tokens[5].second.name() == "source.ruby string.quoted.other.literal.upper.ruby meta.embedded.line.ruby source.ruby" );

        // the 2 consecutive source.ruby is not a bug
        // the author of this syntax made the contentName to be source.ruby
        // and then set one sub-rule as { include: $self }, thus the 2nd source.ruby
        REQUIRE( tokens[6].first.substr(source) == "%Q-crazy ideas-"); 
        REQUIRE( tokens[6].second.name() == "source.ruby string.quoted.other.literal.upper.ruby meta.embedded.line.ruby source.ruby source.ruby string.quoted.other.literal.upper.ruby" );

        REQUIRE( tokens[7].first.substr(source) == "%Q-"); 
        REQUIRE( tokens[7].second.name() == "source.ruby string.quoted.other.literal.upper.ruby meta.embedded.line.ruby source.ruby source.ruby string.quoted.other.literal.upper.ruby punctuation.definition.string.begin.ruby" );

        REQUIRE( tokens[8].first.substr(source) == "-"); 
        REQUIRE( tokens[8].second.name() == "source.ruby string.quoted.other.literal.upper.ruby meta.embedded.line.ruby source.ruby source.ruby string.quoted.other.literal.upper.ruby punctuation.definition.string.end.ruby" );

        REQUIRE( tokens[9].first.substr(source) == "}"); 
        REQUIRE( tokens[9].second.name() == "source.ruby string.quoted.other.literal.upper.ruby meta.embedded.line.ruby punctuation.section.embedded.end.ruby" );

        // below is not a bug
        // in meta.embedded.line.ruby rule, the author specifies
        // endCaptures: {
        //  0: {name: punctuation.section.embedded.end.ruby}, 
        //  1: {name: source.ruby}}
        //  SO this source.ruby is actually child of punctuation.section.embedded.end.ruby
        REQUIRE( tokens[10].first.substr(source) == "}"); 
        REQUIRE( tokens[10].second.name() == "source.ruby string.quoted.other.literal.upper.ruby meta.embedded.line.ruby punctuation.section.embedded.end.ruby source.ruby" );

        REQUIRE( tokens[11].first.substr(source) == "+"); 
        REQUIRE( tokens[11].second.name() == "source.ruby string.quoted.other.literal.upper.ruby punctuation.definition.string.end.ruby" );

        REQUIRE( tokens[12].first.substr(source) == "# damn.\n"); 
        REQUIRE( tokens[12].second.name() == "source.ruby comment.line.number-sign.ruby" );

        REQUIRE( tokens[13].first.substr(source) == "#"); 
        REQUIRE( tokens[13].second.name() == "source.ruby comment.line.number-sign.ruby punctuation.definition.comment.ruby" );
    }
 
    SECTION("include another grammar will work") {
        shared_ptr<TestGrammarSource> grammar_source(new TestGrammarSource({
                "text.html.ruby", "fixture/html-rails.json",
                "source.ruby.rails", "fixture/ruby-on-rails.json",
                "source.ruby", "fixture/ruby.json",
                "text.html.basic", "fixture/html.json"}));
        GrammarRegistry registry(grammar_source);
        Grammar g= registry.get_grammar("text.html.ruby");
        string source = "<div class='name'><%= User.find(2).full_name %></div>";
        auto tokens = tokenizer.tokenize(g, source);


        REQUIRE( tokens[0].first.substr(source) == source ); 
        REQUIRE( tokens[0].second.name() == "text.html.ruby" );

        REQUIRE( tokens[1].first.substr(source) == "<div class='name'>" ); 
        REQUIRE( tokens[1].second.name() == "text.html.ruby text.html.basic meta.tag.block.any.html" );

        REQUIRE( tokens[2].first.substr(source) == "<" ); 
        REQUIRE( tokens[2].second.name() == "text.html.ruby text.html.basic meta.tag.block.any.html punctuation.definition.tag.begin.html" );

        REQUIRE( tokens[3].first.substr(source) == "div" ); 
        REQUIRE( tokens[3].second.name() == "text.html.ruby text.html.basic meta.tag.block.any.html entity.name.tag.block.any.html" );

        REQUIRE( tokens[4].first.substr(source) == "class" ); 
        REQUIRE( tokens[4].second.name() == "text.html.ruby text.html.basic meta.tag.block.any.html entity.other.attribute-name.html" );

        REQUIRE( tokens[5].first.substr(source) == "'name'" ); 
        REQUIRE( tokens[5].second.name() == "text.html.ruby text.html.basic meta.tag.block.any.html string.quoted.single.html" );

        REQUIRE( tokens[6].first.substr(source) == "'" ); 
        REQUIRE( tokens[6].second.name() == "text.html.ruby text.html.basic meta.tag.block.any.html string.quoted.single.html punctuation.definition.string.begin.html" );

        REQUIRE( tokens[7].first.substr(source) == "'" ); 
        REQUIRE( tokens[7].second.name() == "text.html.ruby text.html.basic meta.tag.block.any.html string.quoted.single.html punctuation.definition.string.end.html" );

        REQUIRE( tokens[8].first.substr(source) == ">" ); 
        REQUIRE( tokens[8].second.name() == "text.html.ruby text.html.basic meta.tag.block.any.html punctuation.definition.tag.end.html" );

        REQUIRE( tokens[9].first.substr(source) == "<%= User.find(2).full_name %>" ); 
        REQUIRE( tokens[9].second.name() == "text.html.ruby source.ruby.rails.embedded.html" );

        REQUIRE( tokens[10].first.substr(source) == "<%=" ); 
        REQUIRE( tokens[10].second.name() == "text.html.ruby source.ruby.rails.embedded.html punctuation.section.embedded.ruby" );

        REQUIRE( tokens[11].first.substr(source) == "User" ); 
        REQUIRE( tokens[11].second.name() == "text.html.ruby source.ruby.rails.embedded.html source.ruby.rails source.ruby support.class.ruby" );

        REQUIRE( tokens[12].first.substr(source) == "." ); 
        REQUIRE( tokens[12].second.name() == "text.html.ruby source.ruby.rails.embedded.html source.ruby.rails source.ruby punctuation.separator.method.ruby" );

        REQUIRE( tokens[13].first.substr(source) == "(" ); 
        REQUIRE( tokens[13].second.name() == "text.html.ruby source.ruby.rails.embedded.html source.ruby.rails source.ruby punctuation.section.function.ruby" );

        REQUIRE( tokens[14].first.substr(source) == "2" ); 
        REQUIRE( tokens[14].second.name() == "text.html.ruby source.ruby.rails.embedded.html source.ruby.rails source.ruby constant.numeric.ruby" );

        REQUIRE( tokens[15].first.substr(source) == ")" ); 
        REQUIRE( tokens[15].second.name() == "text.html.ruby source.ruby.rails.embedded.html source.ruby.rails source.ruby punctuation.section.function.ruby" );

        REQUIRE( tokens[16].first.substr(source) == "." ); 
        REQUIRE( tokens[16].second.name() == "text.html.ruby source.ruby.rails.embedded.html source.ruby.rails source.ruby punctuation.separator.method.ruby" );

        REQUIRE( tokens[17].first.substr(source) == "%>" ); 
        REQUIRE( tokens[17].second.name() == "text.html.ruby source.ruby.rails.embedded.html punctuation.section.embedded.ruby" );
    }

    SECTION("external grammar can be optional, if it not exists, we just not tokenize the embeded string") {
        shared_ptr<TestGrammarSource> grammar_source(new TestGrammarSource({
                "source.ruby", "fixture/ruby.json"}));
        GrammarRegistry registry(grammar_source);
        Grammar g= registry.get_grammar("source.ruby");
        string source = "<<-SQL select * from users; SQL";
        auto tokens = tokenizer.tokenize(g, source);

        REQUIRE( tokens.size() == 6 );

        REQUIRE( tokens[0].first.substr(source) == source ); 
        REQUIRE( tokens[0].second.name() == "source.ruby" );

        REQUIRE( tokens[1].first.substr(source) == source ); 
        REQUIRE( tokens[1].second.name() == "source.ruby meta.embedded.block.sql" );

        REQUIRE( tokens[2].first.substr(source) == source ); 
        REQUIRE( tokens[2].second.name() == "source.ruby meta.embedded.block.sql string.unquoted.heredoc.ruby" );

        REQUIRE( tokens[3].first.substr(source) == "<<-SQL" ); 
        REQUIRE( tokens[3].second.name() == "source.ruby meta.embedded.block.sql string.unquoted.heredoc.ruby punctuation.definition.string.begin.ruby" );

        REQUIRE( tokens[4].first.substr(source) == " select * from users;" ); 
        REQUIRE( tokens[4].second.name() == "source.ruby meta.embedded.block.sql string.unquoted.heredoc.ruby source.sql" );

        REQUIRE( tokens[5].first.substr(source) == " SQL" ); 
        REQUIRE( tokens[5].second.name() == "source.ruby meta.embedded.block.sql string.unquoted.heredoc.ruby punctuation.definition.string.end.ruby" );


        shared_ptr<TestGrammarSource> grammar_source1(new TestGrammarSource({
                "source.sql", "fixture/sql.json",
                "source.ruby", "fixture/ruby.json"}));
        GrammarRegistry registry1(grammar_source1);
        Grammar g1 = registry1.get_grammar("source.ruby");
        string source1 = "<<-SQL select * from users; SQL";
        tokens = tokenizer.tokenize(g1, source);

        REQUIRE( tokens.size() == 9 );

        REQUIRE( tokens[0].first.substr(source) == source ); 
        REQUIRE( tokens[0].second.name() == "source.ruby" );

        REQUIRE( tokens[1].first.substr(source) == source ); 
        REQUIRE( tokens[1].second.name() == "source.ruby meta.embedded.block.sql" );

        REQUIRE( tokens[2].first.substr(source) == source ); 
        REQUIRE( tokens[2].second.name() == "source.ruby meta.embedded.block.sql string.unquoted.heredoc.ruby" );

        REQUIRE( tokens[3].first.substr(source) == "<<-SQL" ); 
        REQUIRE( tokens[3].second.name() == "source.ruby meta.embedded.block.sql string.unquoted.heredoc.ruby punctuation.definition.string.begin.ruby" );

        REQUIRE( tokens[4].first.substr(source) == " select * from users;" ); 
        REQUIRE( tokens[4].second.name() == "source.ruby meta.embedded.block.sql string.unquoted.heredoc.ruby source.sql" );

        REQUIRE( tokens[5].first.substr(source) == "select" ); 
        REQUIRE( tokens[5].second.name() == "source.ruby meta.embedded.block.sql string.unquoted.heredoc.ruby source.sql source.sql keyword.other.DML.sql" );

        REQUIRE( tokens[6].first.substr(source) == "*" ); 
        REQUIRE( tokens[6].second.name() == "source.ruby meta.embedded.block.sql string.unquoted.heredoc.ruby source.sql source.sql keyword.operator.star.sql" );

        REQUIRE( tokens[7].first.substr(source) == "from" ); 
        REQUIRE( tokens[7].second.name() == "source.ruby meta.embedded.block.sql string.unquoted.heredoc.ruby source.sql source.sql keyword.other.DML.sql" );

        REQUIRE( tokens[8].first.substr(source) == " SQL" ); 
        REQUIRE( tokens[8].second.name() == "source.ruby meta.embedded.block.sql string.unquoted.heredoc.ruby punctuation.definition.string.end.ruby" );
    }

    SECTION("the grammar can deal with nested c clock") {
        string data = load_string("fixture/c.json");
        string source = "if(1){if(1){m()}}";
        Grammar g = compiler.compile(data);
        compiler.resolve_include(g, nullptr);
        auto tokens = tokenizer.tokenize(g, source);

        REQUIRE( tokens[10].first.substr(source) == "m" );
        REQUIRE( tokens[10].second.name() == "source.c meta.block.c meta.block.c meta.function-call.c support.function.any-method.c" );
        
    }

    SECTION("the back reference in begin/match rule has the normal semantics") {
        string data = load_string("fixture/scss.json");
        string source = "@mixin x() { -moz-selector: whatever; }";
        Grammar g = compiler.compile(data);
        compiler.resolve_include(g, nullptr);
        auto tokens = tokenizer.tokenize(g, source);
    }

    // TODO test external grammar
    // TODO test $base $self
    // TODO hen containing rule has a name
}
