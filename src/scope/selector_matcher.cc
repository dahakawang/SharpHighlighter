#include <algorithm>
#include <shl_exception.h>
#include "selector.h"

namespace shl {
namespace selector {

bool Selector::match(const Scope& scope, double* rank) const {
    if (rank) {
        double max_rank = 0;
        bool matched = false;
        for(const CompositeSelctor& selector : selectors) {
            if (selector.match(scope, rank)) {
                max_rank = std::max(max_rank, *rank);
                matched = true;
            }
        }

        *rank = max_rank;
        return matched;

    } else {
        for (const CompositeSelctor& selector : selectors) {
            if (selector.match(scope, rank)) return true;
        }

        return false;
    }
}

bool CompositeSelctor::match(const Scope& scope, double* rank) const {
    bool matched = selectors[0].match(scope, rank);

    if (rank) {
        double max_rank = *rank;

        for (size_t pos = 1; pos < selectors.size(); pos++) {
            bool matched2 = selectors[pos].match(scope, rank);
            max_rank = std::max(max_rank, *rank);

            switch(operators[pos]) {
                case OR: matched = matched || matched2; break;
                case AND: matched = matched && matched2; break;
                case MINUS: matched = (matched && !matched2); break;
                default: throw InvalidScopeSelector("fatal: an invaid composite selector operator");
            }
        } // for loop
        *rank = max_rank;
        return matched;

    } else {
        for (int pos = 1; pos < selectors.size(); pos++) {
            // we can short-circuit since we don't need to evaluate all selectors for the max rank value
            if (operators[pos] == OR && matched == true) continue;
            if (operators[pos] == AND && matched == false) continue;
            if (operators[pos] == MINUS && matched == false) continue;

            bool matched2 = selectors[pos].match(scope, rank);

            switch(operators[pos]) {
                case OR: matched = matched || matched2; break;
                case AND: matched = matched && matched2; break;
                case MINUS: matched = (matched && !matched2); break;
                default: throw InvalidScopeSelector("fatal: an invaid composite selector operator");
            }
        } // for loop
        return matched;
    }
}

bool ExpressionSelector::match(const Scope& scope, double* rank) const {
    bool matched = selector->match(scope, rank);
    
    return is_negative? !matched : matched;
}

bool FilterSelector::match(const Scope& scope, double* rank) const {
    return false;
}

bool GroupSelector::match(const Scope& scope, double* rank) const {
    return selector.match(scope, rank);
}

bool ScopeSelector::match(const Scope& scope, double* rank) const {
    return false;
}

bool ScopeNameSelector::match(const Scope& scope, double* rank) const {
    return false;
}

}
}
