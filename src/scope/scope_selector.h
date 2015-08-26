#ifndef __SCOPE_SELECTOR__
#define __SCOPE_SELECTOR__

#include <selector.h>
#include <scope.h>

namespace shl {

class Selector {
public:
    Selector();
    Selector(const string& selector);
    bool match(const Scope& scope, int& rank) const;

private:
    selector::Selector;
};

}

#endif
