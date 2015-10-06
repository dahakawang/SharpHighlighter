#include <string.h>
#include <shl_exception.h>
#include "selector.h"

namespace shl {
namespace selector {

bool Selector::match(const Scope& scope, double* rank) const {
    return false;
}

bool CompositeSelctor::match(const Scope& scope, double* rank) const {
    return false;
}

bool ExpressionSelector::match(const Scope& scope, double* rank) const {
    return false;
}

bool FilterSelector::match(const Scope& scope, double* rank) const {
    return false;
}

bool GroupSelector::match(const Scope& scope, double* rank) const {
    return false;
}

bool ScopeSelector::match(const Scope& scope, double* rank) const {
    return false;
}

bool ScopeNameSelector::match(const Scope& scope, double* rank) const {
    return false;
}

}
}
