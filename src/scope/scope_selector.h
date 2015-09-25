#ifndef __SCOPE_SELECTOR__
#define __SCOPE_SELECTOR__

#include <selector.h>
#include <scope.h>

namespace shl {

class Selector {
public:
    typedef selector::Side Side;

    Selector() = default;
    Selector(const string& selector);
    bool match(const Scope& scope, Side side, int& rank) const;

private:
    selector::Selector selector;
};

}

#endif
