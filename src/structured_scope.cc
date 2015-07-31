#include "structured_scope.h"
#include "shl_exception.h"

namespace shl {

StructuredScope::StructuredScope(const string& text, const vector<pair<Range, Scope> >& scopes) {
    if (scopes.size() <= 0) return;

    build(text, scopes, 0, _root, 0);
}

void build(const string& text, const vector<pair<Range, Scope> >& scopes, unsigned pos, ScopeNode& node, unsigned depth) {
    node.name = scopes[pos].second[depth];

    for (; pos < scopes.size(); pos++) {
        
    }
}

}
