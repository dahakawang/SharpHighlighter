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

private:
    vector<string> _scope;
};

}

#endif
