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
};

TestSource::TestSource() {
    ruby = load_string("fixture/ruby.json");
    html = load_string("fixture/html.json");
}

const std::string TestSource::load_data(const std::string& grammar_name) const {
    if ( grammar_name == "text.html.basic" ) return html;
    if ( grammar_name == "source.ruby" ) return ruby;

    return "";
}

const std::vector<std::string> TestSource::list_grammars() const {
    vector<string> list = {"text.html.basic", "source.ruby"};

    return list;
}



TEST_CASE("GrammarRegistry Test") {
    SECTION("can load all grammar presented by the GrammarSource") {
        shared_ptr<GrammarSource> source(new TestSource());
        GrammarRegistry registry(source);

        REQUIRE( registry.contain_grammar("text.html.basic") );
        REQUIRE( registry.contain_grammar("source.ruby") );
    }
}
