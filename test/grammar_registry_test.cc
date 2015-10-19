#include "catch.hpp"
#include "util.h"
#include <grammar_registry.h>

using namespace shl;
using namespace std;

class TestSource : public GrammarSource {
public:
    TestSource();
    virtual const std::string load_data(const std::string& grammar_name) const;
    virtual const std::vector<std::string> list_grammars() const;
private:
    string ruby;
    string html;
    string php;
};

TestSource::TestSource() {
    ruby = load_string("fixture/ruby.json");
    html = load_string("fixture/html.json");
    php = load_string("fixture/php.json");
}

const std::string TestSource::load_data(const std::string& grammar_name) const {
    if ( grammar_name == "text.html.basic" ) return html;
    if ( grammar_name == "source.ruby" ) return ruby;
    if ( grammar_name == "text.html.php" ) return php;

    return "";
}

const std::vector<std::string> TestSource::list_grammars() const {
    vector<string> list = {"text.html.basic", "source.ruby", "text.html.php"};

    return list;
}



TEST_CASE("GrammarRegistry Test") {
    SECTION("can load all grammar presented by the GrammarSource") {
        shared_ptr<GrammarSource> source(new TestSource());
        GrammarRegistry registry(source);

        REQUIRE( registry.contain_grammar("text.html.basic") );
        REQUIRE( registry.contain_grammar("source.ruby") );
    }

    SECTION("all include is resolved") {
        shared_ptr<GrammarSource> source(new TestSource());
        GrammarRegistry registry(source);

        Grammar* html_grammar_ptr  = &registry.get_grammar("text.html.basic");
        Grammar& grammar = registry.get_grammar("source.ruby");

        REQUIRE( grammar.patterns.size() == 88 );
        REQUIRE( grammar.patterns[68].patterns[0].patterns[2].include.name == "source.ruby" );
        REQUIRE( grammar.patterns[68].patterns[0].patterns[2].include.ptr == &grammar );

        REQUIRE( grammar.patterns[59].patterns[0].patterns[2].include.name == "text.html.basic" );
        REQUIRE( grammar.patterns[59].patterns[0].patterns[2].include.ptr == html_grammar_ptr );
    }

    SECTION("all injections is loaded") {
        shared_ptr<GrammarSource> source(new TestSource());
        GrammarRegistry registry(source);

        Grammar& php = registry.get_grammar("text.html.php");
        REQUIRE( !php.empty() );
        REQUIRE( !php.injections.empty() );
    }
}
