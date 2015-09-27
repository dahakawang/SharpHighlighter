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
};


struct ScopeNameSelector : public AbstractSelector {
    vector<string> components;
};

struct ScopeSelector : public AbstractSelector {
    enum AffinityType { DIRECT, NONDIRECT };

    ScopeSelector(): anchor_begin(false), anchor_end(false) {};

    vector<ScopeNameSelector> selectors;
    vector<AffinityType> affinities;
    bool anchor_begin, anchor_end;
};

struct GroupSelector : public AbstractSelector  {
    unique_ptr<AbstractSelector> selector;
};

struct FilterSelector : public AbstractSelector {
    enum Side { Left = 'L', Right = 'R', Both = 'B' };

    FilterSelector(): side(Left) {};

    unique_ptr<AbstractSelector> selector;
    Side side;
};

struct ExpressionSelector : public AbstractSelector {
    ExpressionSelector(): is_negative(false) {};

    unique_ptr<AbstractSelector> selector;
    bool is_negative;
};

struct CompositeSelctor : public AbstractSelector {
    enum CompositionType { OR = '|', AND = '&', MINUS = '-', NONE = 0 };

    vector<ExpressionSelector> selectors;
    vector<CompositionType> operators;
};

struct Selector : public AbstractSelector {
    vector<CompositeSelctor> selectors;
};

}
}

#endif
