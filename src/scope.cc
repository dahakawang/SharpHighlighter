#include <cctype>
#include <string>
#include <vector>
#include <sstream>
#include <cctype>
#include "scope.h"

using std::string;
using std::vector;
using std::ostringstream;

namespace shl {

template<typename T>
vector<string> strtok(const string& str, T predicator) {
    const char* s = str.c_str();
    auto length = str.size();
    vector<string> tokens;

    bool in_separator = true;
    string::size_type start, index;
    for(index = 0; index < length; index++) {
        if (in_separator) {
            if (!predicator(s[index])) {
                in_separator = false;
                start = index;
            }
        } else {
            if (predicator(s[index])) {
                in_separator = true;
                tokens.push_back(str.substr(start, index - start));
            }
        }
    }
    if (!in_separator) {
        tokens.push_back(str.substr(start));
    }

    return tokens;
}
    
Scope::Scope(const string& scope) {
    _scope = strtok(scope, isblank);
}

string trim(const string& str) {
    const char* s = str.c_str();
    string::size_type start, end;
    start = 0;
    end = str.size();

    while(start < end && isblank(s[start])) start++;
    while(end > start && isblank(s[end-1])) end--;

    return str.substr(start, end - start);
}

Scope::Scope(const vector<string>& scope) {
    for (auto& name : scope) {
        if(!trim(name).empty()) _scope.push_back(name);
    }
}

bool Scope::is_prefix_of(const Scope& other) const {
    if (other._scope.size() < _scope.size()) return false;

    for (unsigned i = 0; i < _scope.size(); i++) {
        if (_scope[i] != other._scope[i]) return false;
    }

    return true;
}

vector<string> Scope::breakdown(unsigned pos) const {
    const string& str = _scope[pos];
    return strtok(str, [](char c) { if (c == '.') return true; return false;});
}

Scope Scope::subscope(unsigned pos) const {
    vector<string> scopes(_scope.begin() + pos, _scope.end());

    return Scope(scopes);
}

string Scope::name() const {
    ostringstream name;
    bool first = true;
    for (auto& component : _scope) {
        if (first) {
            first = false;
        } else {
            name << ' ';
        }
        name << component;
    }

    return name.str();
}

}
