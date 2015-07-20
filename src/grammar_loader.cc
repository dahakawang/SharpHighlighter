#include <algorithm>
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
        Pattern* included = find_include(root, grammar, pattern, object.include);
        pattern.include = included;
    } else if (!object.match.empty()) {
        pattern.is_match = true;
        pattern.begin = Regex(object.match);
        pattern.captures = get_captures(object.captures);
    } else if (!object.begin.empty()) {
        pattern.is_match = false;
        pattern.begin = Regex(object.begin);
        pattern.begin_captures = get_captures(object.begin_captures);

        if(!object.end.empty()) throw InvalidGrammarException("should have end for a begin/end pattern");
        pattern.end = Regex(object.end);
        pattern.end_captures = get_captures(object.end_captures);
        pattern.content_name = object.content_name;

        pattern.patterns = vector<Pattern>(object.patterns.size());
        for (int idx = 0; idx < pattern.patterns.size(); idx++) {
            compile_grammar(root, grammar, object.patterns[idx], pattern.patterns[idx]);
        }
    }
}

int to_int(const string& str) {
    for (int idx = 0; idx < str.size(); idx++) {
        if (!isdigit(str[idx])) throw InvalidGrammarException("only numeric capture keys are supported");
    }
    return atoi(str.c_str());
}

string to_capture_name(const map<string, string> m) {
    auto it = m.find("name");
    if (it == m.end()) throw InvalidGrammarException("capture should have the name property");
    return it->second;
}

map<int, string> GrammarLoader::get_captures(const map<string, map<string, string> > raw_capture) {
    map<int, string> captures;
    for(auto raw_it = raw_capture.begin(); raw_it != raw_capture.end(); raw_it++) {
        int id = to_int(raw_it->first);
        string name = to_capture_name(raw_it->second);
    }

    return captures;
}

Pattern* GrammarLoader::find_include(const JsonObject& root, Grammar& grammar, Pattern& pattern, const string& include_name) {
    
    // self reference
    if (include_name == "$self") {
        return &pattern;

    // repository reference
    } else if (include_name[0] == '#'){
        // if the stocked resource is already added, then return the address directly
        auto stock_res = grammar.repository.find(include_name);
        if (stock_res != grammar.repository.end()) return &stock_res->second;

        auto it = root.repository.find(include_name);
        if (it == root.repository.end()) throw InvalidGrammarException("can't find the name in repository");
        grammar.repository[include_name] = Pattern();
        compile_grammar(root, grammar, it->second, grammar.repository[include_name]);
        return &grammar.repository[include_name];

    // external grammar reference
    } else {
        throw InvalidGrammarException("include another grammar is not supported yet");
    }

}

}
