#ifndef _STRUCTURED_SCOPE_
#define _STRUCTURED_SCOPE_

#include <string>
#include <vector>
#include <utility>
#include "scope.h"
#include "regex.h"

using std::string;
using std::vector;
using std::pair;

namespace shl {

struct ScopeNode {
    string name;
    vector<ScopeNode> children;
    string content;
};

class StructuredScope {
public:
    StructuredScope(const string& text, const vector<pair<Range, Scope> >& scopes);
    string to_markup() const;

private:
    ScopeNode _root;

    void build(const string& text, const vector<pair<Range, Scope> >& scopes, unsigned pos, ScopeNode& node, unsigned depth);
};

}
#endif
