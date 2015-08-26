#ifndef __GRAMMAR_COMPILER__
#define __GRAMMAR_COMPILER__

#include <string>
#include "grammar.h"
#include "json_object.h"
#include "grammar_registry.h"

namespace shl {

class GrammarCompiler {
public:
    Grammar compile(const string& buffer);
    void resolve_include(Grammar& grammar, GrammarRegistry* registry);

private:
    void process(const JsonObject& object, Grammar& grammar);
    void compile_grammar(const JsonObject& object, Rule& rule);
    map<int, string> get_captures(const map<string, map<string, string> > raw_capture);
};

}

#endif
