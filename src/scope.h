#ifndef _SCOPE_H_
#define _SCOPE_H_

#include <vector>
#include <string>

using std::string;
using std::vector;

namespace shl {

class ScopeName {
public:
    ScopeName() = default;
    ScopeName(const string& scope_name);
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
    Scope() {};
    Scope(const vector<string>& scope);
    Scope(const vector<ScopeName>& scope);
    Scope(const string& scope);
    string name() const;
    bool is_prefix_of(const Scope& other) const;
    unsigned size() const { return _scope.size(); };
    ScopeName operator[](unsigned pos) const { return _scope[pos]; };
    bool operator==(const Scope& other) const;
    bool operator!=(const Scope& other) const;
    Scope subscope(unsigned pos) const;

private:
    vector<ScopeName> _scope;
};

}

#endif
