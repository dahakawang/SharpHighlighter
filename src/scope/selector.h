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
    virtual string str() const = 0;
};


struct ScopeNameSelector : public AbstractSelector {
    ScopeNameSelector(): anchor_prev(false) {};
    virtual bool match(const Scope& scope, char side, double* rank) const;
    virtual string str() const;

    bool anchor_prev;
    vector<string> components;
};

struct ScopeSelector : public AbstractSelector {
    ScopeSelector(): anchor_begin(false), anchor_end(false) {};
    virtual bool match(const Scope& scope, char side, double* rank) const;
    virtual string str() const;

    vector<ScopeNameSelector> atoms;
    bool anchor_begin, anchor_end;
};


struct FilterSelector : public AbstractSelector {
    enum Side { Left = 'L', Right = 'R', Both = 'B', None = 'N' };

    FilterSelector(): side(Left) {};
    virtual bool match(const Scope& scope, char side, double* rank) const;
    virtual string str() const;

    std::shared_ptr<AbstractSelector> selector;
    Side side;
};

struct ExpressionSelector : public AbstractSelector {
    ExpressionSelector(): is_negative(false) {};
    virtual bool match(const Scope& scope, char side, double* rank) const;
    virtual string str() const;

    FilterSelector selector;
    bool is_negative;
};

struct CompositeSelctor : public AbstractSelector {
    enum CompositionType { OR = '|', AND = '&', MINUS = '-', NONE = 0 };

    virtual bool match(const Scope& scope, char side, double* rank) const;
    virtual string str() const;

    vector<ExpressionSelector> selectors;
    vector<CompositionType> operators;
};

struct Selector : public AbstractSelector {
    virtual bool match(const Scope& scope, char side, double* rank) const;
    virtual string str() const;

    vector<CompositeSelctor> selectors;
};

struct GroupSelector : public AbstractSelector  {
    virtual bool match(const Scope& scope, char side, double* rank) const;
    virtual string str() const;

    Selector selector;
};

}
}

#endif
