#ifndef _GRAMMAR_H_
#define _GRAMMAR_H_

#include <string>
#include <vector>
#include <map>
#include <shl_regex.h>

using std::string;
using std::map;
using std::vector;

namespace shl {

class Rule;

class WeakIncludePtr {
public:
    WeakIncludePtr():ptr(nullptr), is_base_ref(false) {};
    WeakIncludePtr(string _name):name(_name), ptr(nullptr), is_base_ref(false) {};
    Rule* ptr;
    bool is_base_ref;
    string name;
};

class Rule {
public:
    Rule():is_match_rule(false), applyEndPatternLast(false) {};
    bool is_match_rule;
    string name;
    Regex begin;
    EndPatternRegex end;
    bool applyEndPatternLast;
    map<int, string> begin_captures;
    map<int, string> end_captures;
    vector<Rule> patterns;
    string content_name;
    WeakIncludePtr include; // like a kind of weak pointer, only points to entries in repository
};

class Grammar : public Rule {
public:
    string desc;
    vector<string> file_types;
    map<string, Rule> repository;

    bool empty() { return name.empty() && patterns.empty() && repository.empty(); };
};
}


#endif
