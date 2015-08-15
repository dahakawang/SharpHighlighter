#ifndef _GRAMMAR_LOADER_
#define _GRAMMAR_LOADER_

#include <string>
#include "grammar.h"
#include "json_object.h"

namespace shl {

class GrammarLoader {
public:
    Grammar load(const string& buffer);

private:
    void process(const JsonObject& object, Grammar& grammar);
    void compile_grammar(const JsonObject& root, Grammar& grammar, const JsonObject& object, Rule& rule, Rule* parent);
    map<int, string> get_captures(const map<string, map<string, string> > raw_capture);
    WeakIncludePtr find_include(const JsonObject& root, Grammar& grammar, const string& include_name);
};

}

#endif
