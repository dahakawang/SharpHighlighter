#include <string>
#include <map>
#include <iterator>
#include <grammar_compiler.h>
#include <shl_exception.h>
#include "grammar_registry.h"

using std::string;

namespace shl {

static void resolve_all_include(map<string, Grammar>& grammars, GrammarRegistry* registry) {
    GrammarCompiler compiler;

    for ( auto& name_grammar : grammars ) {
        Grammar& grammar = name_grammar.second;
        compiler.resolve_include(grammar, registry);
    }
}
GrammarRegistry::GrammarRegistry(const std::shared_ptr<GrammarSource> data_source):_source(data_source) {
    auto list = _source->list_grammars();

    for ( auto& grammar_name : list ) {
        string raw_grammar = _source->load_data(grammar_name);
        add_grammar(raw_grammar);
    }
    resolve_all_include(_grammars, this);
}

Grammar& GrammarRegistry::add_grammar(const std::string& data) {
    if (data.empty()) return _empty;
    GrammarCompiler compiler;
    Grammar grammar = compiler.compile(data);
    string grammar_name = grammar.name;
    
    _grammars[grammar_name] = std::move(grammar);
    return _grammars[grammar_name];
}

bool GrammarRegistry::contain_grammar(const std::string& grammar_name) const {
    return _grammars.count(grammar_name) != 0;
}

void GrammarRegistry::load_grammar(const std::string& grammar_name) {
    if (contain_grammar(grammar_name)) return;

    string raw_grammar = _source->load_data(grammar_name);
    add_grammar(raw_grammar);
    resolve_all_include(_grammars, this);
}

const Grammar& GrammarRegistry::get_grammar(const std::string& grammar_name) const {
    if (_grammars.count(grammar_name) == 0) return _empty;

    return _grammars.find(grammar_name)->second;
}

Grammar& GrammarRegistry::get_grammar(const std::string& grammar_name) {
    return const_cast<Grammar&>(static_cast<const GrammarRegistry*>(this)->get_grammar(grammar_name));
}

}
