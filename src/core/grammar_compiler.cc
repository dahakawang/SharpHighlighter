#include <algorithm>
#include <grammar.h>
#include <json_object.h>
#include <json_loader.h>
#include <shl_exception.h>
#include "grammar_compiler.h"

namespace shl {

Grammar GrammarCompiler::compile(const string& buffer) {
    JsonLoader loader;
    JsonObject object = loader.load(buffer);

    Grammar grammar;
    process(object, grammar);
    return grammar;
}

void GrammarCompiler::process(const JsonObject& object, Grammar& grammar) {
    grammar.desc = object.scope_name;
    grammar.file_types = object.file_types;

    compile_grammar(object, grammar);
    swap(grammar.desc, grammar.name);
}

void GrammarCompiler::compile_grammar(const JsonObject& object, Rule& rule) {
    rule.name = object.name;

    if (!object.include.empty()) {
        rule.include = WeakIncludePtr(object.include);
    } else if (!object.match.empty()) {
        rule.is_match_rule = true;
        rule.begin = Regex(object.match);
        rule.begin_captures = get_captures(object.captures);
    } else if (!object.begin.empty()) {
        rule.is_match_rule = false;
        rule.begin = Regex(object.begin);
        rule.begin_captures = get_captures(object.begin_captures);
        if (rule.begin_captures.empty()) rule.begin_captures = get_captures(object.captures);

        // TODO warning if end rule is empty
        rule.end = EndPatternRegex(object.end);
        rule.end_captures = get_captures(object.end_captures);
        if (rule.end_captures.empty()) rule.end_captures = get_captures(object.captures);
        rule.content_name = object.content_name;
        if (object.applyEndPatternLast == "true") rule.applyEndPatternLast = true;
    } else {
        if (object.patterns.empty()) {
            // TODO warning if empty rule is met
        }
    }

    // compile repository & injections
    for ( auto& stocked : object.repository ) {
        const string& repo_name = stocked.first;
        const JsonObject& root = stocked.second;

        rule.repository[repo_name] = Rule();
        compile_grammar(root, rule.repository[repo_name]);
    }
    for ( auto& stocked : object.injections ) {
        const string& repo_name = stocked.first;
        const JsonObject& root = stocked.second;

        rule.injections[repo_name] = Rule();
        compile_grammar(root, rule.injections[repo_name]);
    }

    rule.patterns = vector<Rule>(object.patterns.size());
    for (unsigned int idx = 0; idx < rule.patterns.size(); idx++) {
        compile_grammar(object.patterns[idx], rule.patterns[idx]);
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

map<int, string> GrammarCompiler::get_captures(const map<string, map<string, string> > raw_capture) {
    map<int, string> captures;
    for(auto raw_it = raw_capture.begin(); raw_it != raw_capture.end(); raw_it++) {
        int id = to_int(raw_it->first);
        string name = to_capture_name(raw_it->second);
        captures[id] = name;
    }

    return captures;
}

static Rule* find_repository_res(Rule& rule, const string& repo_name) {
    Rule* it = &rule;
    while (it != nullptr) {
        auto resource = it->repository.find(repo_name);
        if ( resource != it->repository.end() ) {
            return &resource->second;
        }
        it = it->lex_parent;
    }
    
    throw InvalidGrammarException(string("can't find the name in repository: ") + repo_name);
}

static void find_include(GrammarRegistry* registry, Grammar& grammar, Rule& rule) {
    WeakIncludePtr& include_rule = rule.include;
    string include_name = include_rule.name;
    
    // base reference
    if (include_name == "$base") {
        include_rule.is_base_ref = true;

    // self reference
    } else if (include_name == "$self") {
        include_rule.ptr = &grammar;

    // repository reference
    } else if (include_name[0] == '#'){
        string repo_name = include_name.substr(1);
        include_rule.ptr = find_repository_res(rule, repo_name);

    // external grammar reference
    } else {
        if (registry == nullptr) {
            throw ShlException("can't refer to external grammar without grammar registry");
        } else {
            //TODO report if no external grammar found
            Grammar& embeded_grammar = registry->get_grammar(include_name);
            include_rule.ptr = &embeded_grammar;
        }
    }

}

static void _resolve_include(Grammar& grammar, Rule& rule, GrammarRegistry* registry) {
    if ( rule.include.name.empty() ) {
        for ( Rule& subrule : rule.patterns ) {
            subrule.lex_parent = &rule;
            _resolve_include(grammar, subrule, registry);
        }
    } else {
        find_include(registry, grammar, rule);
    }
}

void GrammarCompiler::resolve_include(Grammar& grammar, GrammarRegistry* registry) { 
    grammar.lex_parent = nullptr;
    _resolve_include(grammar, grammar, registry);

    for ( auto& stocked_rule : grammar.repository ) {
        stocked_rule.second.lex_parent = &grammar;
        _resolve_include(grammar, stocked_rule.second, registry);
    }
}
}
