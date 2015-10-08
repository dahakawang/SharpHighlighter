#ifndef __SELECTOR__
#define __SELECTOR__

#include <vector>
#include <memory>
#include "scope.h"

using std::vector;
using std::unique_ptr;

namespace shl {
namespace selector {

struct AbstractSelector {
    virtual bool match(const Scope& scope, char side, double* rank) const = 0;
};


struct ScopeNameSelector : public AbstractSelector {
    ScopeNameSelector(): anchor_prev(false) {};
    virtual bool match(const Scope& scope, char side, double* rank) const;

    bool anchor_prev;
    vector<string> components;
};

struct ScopeSelector : public AbstractSelector {
    ScopeSelector(): anchor_begin(false), anchor_end(false) {};
    virtual bool match(const Scope& scope, char side, double* rank) const;

    vector<ScopeNameSelector> atoms;
    bool anchor_begin, anchor_end;
};


struct FilterSelector : public AbstractSelector {
    enum Side { Left = 'L', Right = 'R', Both = 'B' };

    FilterSelector(): side(Left) {};
    virtual bool match(const Scope& scope, char side, double* rank) const;

    unique_ptr<AbstractSelector> selector;
    Side side;
};

struct ExpressionSelector : public AbstractSelector {
    ExpressionSelector(): is_negative(false) {};
    virtual bool match(const Scope& scope, char side, double* rank) const;

    unique_ptr<AbstractSelector> selector;
    bool is_negative;
};

struct CompositeSelctor : public AbstractSelector {
    enum CompositionType { OR = '|', AND = '&', MINUS = '-', NONE = 0 };

    virtual bool match(const Scope& scope, char side, double* rank) const;

    vector<ExpressionSelector> selectors;
    vector<CompositionType> operators;
};

struct Selector : public AbstractSelector {
    virtual bool match(const Scope& scope, char side, double* rank) const;

    vector<CompositeSelctor> selectors;
};

struct GroupSelector : public AbstractSelector  {
    virtual bool match(const Scope& scope, char side, double* rank) const;

    Selector selector;
};

}
}

#endif
