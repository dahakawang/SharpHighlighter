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
    grammar.desc = object.scope_name;
    grammar.file_types = object.file_types;
    compile_grammar(object, grammar, object, grammar, nullptr);
    swap(grammar.desc, grammar.name);
}

void GrammarLoader::compile_grammar(const JsonObject& root, Grammar& grammar, const JsonObject& object, Rule& rule, Rule* parent) {
    rule.name = object.name;

    if (!object.include.empty()) {
        Rule* included = find_include(root, grammar, rule, object.include, parent);
        rule.include = included;
    } else if (!object.match.empty()) {
        rule.is_match_rule = true;
        rule.begin = Regex(object.match);
        rule.begin_captures = get_captures(object.captures);
    } else if (!object.begin.empty()) {
        rule.is_match_rule = false;
        rule.begin = Regex(object.begin);
        rule.begin_captures = get_captures(object.begin_captures);
        if (rule.begin_captures.empty()) rule.begin_captures = get_captures(object.captures);

        if(object.end.empty()) throw InvalidGrammarException("should have end for a begin/end pattern");
        rule.end = EndPatternRegex(object.end);
        rule.end_captures = get_captures(object.end_captures);
        if (rule.end_captures.empty()) rule.end_captures = get_captures(object.captures);
        rule.content_name = object.content_name;
        if (object.applyEndPatternLast == "true") rule.applyEndPatternLast = true;
    } else {
        if (object.patterns.empty()) {
            throw InvalidGrammarException("empty rule is not alowed");
        } else {
            if (parent != nullptr && parent->begin.empty()) {
                throw InvalidGrammarException("a containing rule can't be direct child of abother containing rule");
            }
        }
    }

    rule.patterns = vector<Rule>(object.patterns.size());
    for (unsigned int idx = 0; idx < rule.patterns.size(); idx++) {
        compile_grammar(root, grammar, object.patterns[idx], rule.patterns[idx], &rule);
    }
}

int to_int(const string& str) {
    for (unsigned int idx = 0; idx < str.size(); idx++) {
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
        captures[id] = name;
    }

    return captures;
}

Rule* GrammarLoader::find_include(const JsonObject& root, Grammar& grammar, Rule& rule, const string& include_name, Rule* parent) {
    
    // base reference
    if (include_name == "$base") {
        return &grammar;

    // self reference
    } else if (include_name == "$self") {
        if ( parent != nullptr) {
            return parent;
        } else {
            throw InvalidGrammarException("a toplevel grammar or toplevel repository item can't included $base");
        }

    // repository reference
    } else if (include_name[0] == '#'){
        string repo_name = include_name.substr(1);
        // if the stocked resource is already added, then return the address directly
        auto stock_res = grammar.repository.find(repo_name);
        if (stock_res != grammar.repository.end()) return &stock_res->second;

        auto it = root.repository.find(repo_name);
        if (it == root.repository.end()) throw InvalidGrammarException(string("can't find the name in repository: ") + repo_name);
        grammar.repository[repo_name] = Rule();
        compile_grammar(root, grammar, it->second, grammar.repository[repo_name], nullptr);
        return &grammar.repository[repo_name];

    // external grammar reference
    } else {
        throw InvalidGrammarException("include another grammar is not supported yet");
    }

}

}
