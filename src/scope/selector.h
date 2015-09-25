#ifndef __SELECTOR__
#define __SELECTOR__

#include <vector>

using std::vector;

namespace shl {
namespace selector {

struct Selector {
    vector<CompositeSelctor> composites;
};

struct CompositeSelctor {
    enum CompositionType { OR, AND, MINUS };
};

struct ExpressionSelector {
};

struct FilterSelector {
};

struct GroupSelector {
};

struct ScopeSelector {
};

struct ScopeNameSelector {
};

}
}

#endif
