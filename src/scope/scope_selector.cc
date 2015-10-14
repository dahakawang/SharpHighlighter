#include <selector.h>
#include <selector_parser.h>
#include "scope_selector.h"

namespace shl {

Selector::Selector(const string& selector) {
    selector::Parser parser(selector);
    this->selector = parser.parse();
}

bool Selector::match(const Scope& scope, Side side, double* rank) const {
    return selector.match(scope, side, rank);
}

bool Selector::match(const string& scope, Side side, double* rank) const {
    return selector.match(Scope(scope), side, rank);
}

}
