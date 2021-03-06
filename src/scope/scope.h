#ifndef _SCOPE_H_
#define _SCOPE_H_

#include <vector>
#include <string>
#include <shl_regex.h>

using std::string;
using std::vector;

namespace shl {

class ScopeName {
public:
    ScopeName() = default;
    ScopeName(const string& scope_name, const string& text = "", const Match& match = Match());
    const string& name() const { return _name; };
    const vector<string>& breakdown() const { return _component; };
    bool operator==(const ScopeName& rh) const { return _name == rh._name;};
    bool operator!=(const ScopeName& rh) const { return _name != rh._name;};

private:
    vector<string> _component;
    string _name;
};

class Scope {
public:
    Scope() = default;
    Scope(const vector<string>& scope, const string& text = "", const Match& match = Match());
    Scope(const vector<ScopeName>& scope);
    Scope(const string& scope, const string& text = "", const Match& match = Match());
    const string name() const;
    bool is_prefix_of(const Scope& other) const;
    unsigned size() const { return _scope.size(); };
    const ScopeName& operator[](unsigned pos) const { return _scope[pos]; };
    const ScopeName& back() const { return _scope.back(); };
    const ScopeName& front() const { return _scope.front(); };
    bool operator==(const Scope& other) const;
    bool operator!=(const Scope& other) const;
    const Scope subscope(unsigned pos) const;

private:
    vector<ScopeName> _scope;
};

}

#endif
