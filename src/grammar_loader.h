#ifndef _GRAMMAR_LOADER_
#define _GRAMMAR_LOADER_

#include <string>
#include "grammar.h"
#include "json_object.h"

namespace shl {

class GrammarLoader {
public:
    Grammar load(const string& buffer);
    void process(const JsonObject& object, Grammar& grammar);
    void compile_grammar(const JsonObject& root, Grammar& grammar, const JsonObject& object, Pattern& pattern);

};

}

#endif
