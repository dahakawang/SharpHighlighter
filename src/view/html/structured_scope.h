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
    Scope name;
    vector<ScopeNode> children;
    string content;
};

class StructuredScope {
public:
    string to_markup(const string& text, const vector<pair<Range, Scope> >& scopes);

private:
    unsigned build(const string& text, const vector<pair<Range, Scope> >& scopes, unsigned pos, unsigned depth, string& markup);
};

}
#endif
