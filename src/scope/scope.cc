#include <cctype>
#include <string>
#include <vector>
#include <sstream>
#include <cctype>
#include <scope_macro.h>
#include "scope.h"
#include "str_util.h"

using std::string;
using std::vector;
using std::ostringstream;

namespace shl {

ScopeName::ScopeName(const string& scope_name, const string& text, const Match& match) {
    _name = scope_name;
    _component = strtok(scope_name, [](char c) { return c == '.';});

    for(size_t pos = 0; pos < _component.size(); pos++) {
        _component[pos] = expand_macro(_component[pos], text, match);
    }
}

Scope::Scope(const string& scope, const string& text, const Match& match) {
    auto scope_str = strtok(scope, isblank);

    for(auto it = scope_str.begin(); it != scope_str.end(); it++) {
        _scope.emplace_back(ScopeName(*it, text, match));
    }
}


Scope::Scope(const vector<string>& scope, const string& text, const Match& match) {
    for (auto& name : scope) {
        if(!trim(name).empty()) _scope.emplace_back(ScopeName(name, text, match));
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

const Scope Scope::subscope(unsigned pos) const {
    vector<ScopeName> scopes(_scope.begin() + pos, _scope.end());

    return Scope(scopes);
}

const string Scope::name() const {
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
