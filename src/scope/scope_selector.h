#ifndef __SCOPE_SELECTOR__
#define __SCOPE_SELECTOR__

#include <selector.h>
#include <scope.h>

namespace shl {

class Selector {
public:
    enum Side { LEFT = 'L', RIGHT = 'R', BOTH = 'B'};
    Selector() = default;
    Selector(const string& selector);
    bool match(const Scope& scope, Side side = BOTH, double* rank = nullptr) const;
    bool match(const string& scope, Side side = BOTH, double* rank = nullptr) const;
    string str() const { return selector.str(); };

private:
    selector::Selector selector;
};

}

#endif
