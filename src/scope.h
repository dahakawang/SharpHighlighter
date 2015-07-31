#ifndef _SCOPE_H_
#define _SCOPE_H_

#include <vector>
#include <string>

using std::string;
using std::vector;

namespace shl {

class Scope {
public:
    Scope(const vector<string>& scope);
    Scope(const string& scope);
    string name() const;
    bool is_prefix_of(const Scope& other) const;
    unsigned size() const { return _scope.size(); };
    string operator[](unsigned pos) const { return _scope[pos]; };

private:
    vector<string> _scope;
};

}

#endif
