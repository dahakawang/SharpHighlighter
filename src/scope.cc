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

string trim(const string& str) {
    const char* s = str.c_str();
    string::size_type start, end;
    start = 0;
    end = str.size();

    while(start < end && isblank(s[start])) start++;
    while(end > start && isblank(s[end-1])) end--;

    return str.substr(start, end - start);
}

ScopeName::ScopeName(const string& scope_name) {
    _name = scope_name;
    _component = strtok(scope_name, [](char c) { return c == '.';});
}

Scope::Scope(const string& scope) {
    auto scope_str = strtok(scope, isblank);

    for(auto it = scope_str.begin(); it != scope_str.end(); it++) {
        _scope.emplace_back(*it);
    }
}


Scope::Scope(const vector<string>& scope) {
    for (auto& name : scope) {
        if(!trim(name).empty()) _scope.push_back(name);
    }
}

Scope::Scope(const vector<ScopeName>& scope):_scope(scope) {
}

bool Scope::is_prefix_of(const Scope& other) const {
    if (other._scope.size() < _scope.size()) return false;

    for (unsigned i = 0; i < _scope.size(); i++) {
        if (_scope[i] != other._scope[i]) return false;
    }

    return true;
}

bool Scope::operator==(const Scope& other) const {
    if (_scope.size() != other._scope.size()) return false;

    for (unsigned i = 0; i < _scope.size(); i++) {
        if (other._scope[i] != _scope[i]) return false;
    }

    return true;
}

bool Scope::operator!=(const Scope& other) const {
    return ! (*this == other);
}

Scope Scope::subscope(unsigned pos) const {
    vector<ScopeName> scopes(_scope.begin() + pos, _scope.end());

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
        name << component.name();
    }

    return name.str();
}

}
