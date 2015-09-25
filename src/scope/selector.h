#ifndef __SELECTOR__
#define __SELECTOR__

#include <vector>
#include <memory>
#include "scope.h"

using std::vector;
using std::unique_ptr;

namespace shl {
namespace selector {


enum Side { Left, Right, Both };


class AbstractSelector {
public:
    virtual bool match(const Scope& scope, Side side, int& rank) const = 0;
    virtual bool empty() const = 0;
};


class ScopeNameSelector {
public:
    virtual bool match(const Scope& scope, Side side, int& rank) const;

private:
    vector<string> components;
};

class ScopeSelector : public AbstractSelector {
public:
    enum AffinityType { DIRECT, NONDIRECT };
    virtual bool match(const Scope& scope, Side side, int& rank) const;

private:
    vector<ScopeNameSelector> selectors;
    vector<AffinityType> affinities;
    bool anchor_begin, anchor_end;
};

class GroupSelector : public AbstractSelector  {
public:
    virtual bool match(const Scope& scope, Side side, int& rank) const;

private:
    unique_ptr<AbstractSelector> selector;
};

class FilterSelector : public AbstractSelector {
public:
    virtual bool match(const Scope& scope, Side side, int& rank) const;

private:
    unique_ptr<AbstractSelector> selector;
    Side side;
};

class ExpressionSelector : public AbstractSelector {
public:
    ExpressionSelector(): is_negative(false) {};
    virtual bool match(const Scope& scope, Side side, int& rank) const;
    virtual bool empty() const { return selector == nullptr; };
    static ExpressionSelector parse(const string& str, size_t& pos);

private:
    unique_ptr<AbstractSelector> selector;
    bool is_negative;
};

class CompositeSelctor : public AbstractSelector {
public:
    enum CompositionType { OR = '|', AND = '&', MINUS = '-', NONE = 0 };

    virtual bool match(const Scope& scope, Side side, int& rank) const;
    virtual bool empty() const { return selectors.empty(); };
    static CompositeSelctor parse(const string& str, size_t& pos);

private:
    vector<ExpressionSelector> selectors;
    vector<CompositionType> operators;
};

class Selector : public AbstractSelector {
public:
    virtual bool match(const Scope& scope, Side side, int& rank) const;
    virtual bool empty() const { return selectors.empty(); };
    static Selector parse(const string& str, size_t& pos);

private:
    vector<CompositeSelctor> selectors;
};

}
}

#endif
