#include "grammar_registry.h"
#include "grammar_compiler.h"
#include "shl_exception.h"
#include <string>
#include <map>
#include <iterator>

using std::string;

namespace shl {

GrammarRegistry::GrammarRegistry(const std::shared_ptr<GrammarSource> data_source):_source(data_source) {
    auto list = _source->list_grammars();

    for ( auto grammar_name : list ) {
        string raw_grammar = _source->load_data(grammar_name);
        add_grammar(raw_grammar);
    }
}

Grammar& GrammarRegistry::add_grammar(const std::string& data) {
    if (data.empty()) return _empty;
    GrammarCompiler compiler;
    Grammar grammar = compiler.compile(data);
    string& grammar_name = grammar.name;
    
    _grammars[grammar_name] = std::move(grammar);
    return _grammars[grammar_name];
}

bool GrammarRegistry::contain_grammar(const std::string& grammar_name) const {
    return _grammars.count(grammar_name) != 0;
}

Grammar& GrammarRegistry::load_grammar(const std::string& grammar_name) {
    if (_grammars.count(grammar_name) != 0) return _grammars.find(grammar_name)->second;

    string raw_grammar = _source->load_data(grammar_name);
    return add_grammar(raw_grammar);
}

const Grammar& GrammarRegistry::get_grammar(const std::string& grammar_name) const {
    if (_grammars.count(grammar_name) == 0) return _empty;

    return _grammars.find(grammar_name)->second;
}

}
