#ifndef _GRAMMAR_H_
#define _GRAMMAR_H_

#include <string>
#include <vector>
#include <map>
#include "regex.h"

using std::string;
using std::map;
using std::vector;

namespace shl {

class Pattern {
public:
    Pattern():include(nullptr) {};
    bool is_match_rule;
    string name;
    Regex begin;
    map<int, string> captures;
    Regex end;
    bool applyEndPatternLast;
    map<int, string> begin_captures;
    map<int, string> end_captures;
    vector<Pattern> patterns;
    string content_name;
    Pattern* include; // like a kind of weak pointer, only points to entries in repository
};

class Grammar : public Pattern {
public:
    string desc;
    vector<string> file_types;
    map<string, Pattern> repository;
};
}


#endif
