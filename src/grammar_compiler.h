#ifndef __GRAMMAR_COMPILER__
#define __GRAMMAR_COMPILER__

#include <string>
#include "grammar.h"
#include "json_object.h"

namespace shl {

class GrammarCompiler {
public:
    Grammar compile(const string& buffer);

private:
    void process(const JsonObject& object, Grammar& grammar);
    void compile_grammar(const JsonObject& root, Grammar& grammar, const JsonObject& object, Rule& rule, Rule* parent);
    map<int, string> get_captures(const map<string, map<string, string> > raw_capture);
    WeakIncludePtr find_include(const JsonObject& root, Grammar& grammar, const string& include_name);
};

}

#endif
