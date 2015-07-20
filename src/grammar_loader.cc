#include "grammar_loader.h"
#include "grammar.h"
#include "json_object.h"
#include "json_loader.h"
#include "shl_exception.h"

namespace shl {

Grammar GrammarLoader::load(const string& buffer) {
    JsonLoader loader;
    JsonObject object = loader.load(buffer);

    Grammar grammar;
    process(object, grammar);
    return grammar;
}

void GrammarLoader::process(const JsonObject& object, Grammar& grammar) {
    grammar.desc = object.name;
    grammar.file_types = object.file_types;
    compile_grammar(object, grammar, object, grammar);
    swap(grammar.desc, grammar.name);
}

void GrammarLoader::compile_grammar(const JsonObject& root, Grammar& grammar, const JsonObject& object, Pattern& pattern) {
    pattern.name = object.name;

    if (!object.include.empty()) {
        Pattern* included = nullptr;//find_include(object.include);
        pattern.include = included;
    } else if (!object.match.empty()) {
        pattern.is_match = true;
        pattern.begin = Regex(object.match);
        //pattern.captures = get_captures(object.captures);
    } else if (!object.begin.empty()) {
        pattern.is_match = false;
        pattern.begin = Regex(object.begin);
        //pattern.begin_captures = get_captures(object.begin_captures);

        if(!object.end.empty()) throw InvalidGrammarException("should have end for a begin/end pattern");
        pattern.end = Regex(object.end);
        //pattern.end_captures = get_captures(object.end_captures);
        pattern.content_name = object.content_name;
    }
}
}
