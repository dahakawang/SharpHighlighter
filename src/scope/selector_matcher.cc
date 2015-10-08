#include <algorithm>
#include <shl_exception.h>
#include "selector.h"

namespace shl {
namespace selector {

bool Selector::match(const Scope& scope, char side, double* rank) const {
    if (rank) {
        double max_rank = 0;
        bool matched = false;
        for(const CompositeSelctor& selector : selectors) {
            if (selector.match(scope, side, rank)) {
                max_rank = std::max(max_rank, *rank);
                matched = true;
            }
        }

        *rank = max_rank;
        return matched;

    } else {
        for (const CompositeSelctor& selector : selectors) {
            if (selector.match(scope, side, rank)) return true;
        }

        return false;
    }
}

bool CompositeSelctor::match(const Scope& scope, char side, double* rank) const {
    bool matched = selectors[0].match(scope, side, rank);

    if (rank) {
        double max_rank = *rank;

        for (size_t pos = 1; pos < selectors.size(); pos++) {
            bool matched2 = selectors[pos].match(scope, side, rank);
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

            bool matched2 = selectors[pos].match(scope, side, rank);

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

bool ExpressionSelector::match(const Scope& scope, char side, double* rank) const {
    bool matched = selector->match(scope, side, rank);
    
    return is_negative? !matched : matched;
}

bool FilterSelector::match(const Scope& scope, char side, double* rank) const {
    if (side == this->side) return selector->match(scope, side, rank);
    return false;
}

bool GroupSelector::match(const Scope& scope, char side, double* rank) const {
    return selector.match(scope, side, rank);
}

bool match_atom_prefix(const ScopeNameSelector& selector, const ScopeName& atom, double* rank) {
    double score = 0;

    auto& _selector = selector.components;
    auto& _atom = atom.breakdown();
    for(size_t it = 0; it < _selector.size(); it++) {
        if(it < _atom.size() && (_selector[it] == "*" || _selector[it] == _atom[it])) {
            score += ((_selector[it] == "*")? 0.5 : 1);
        } else {
            *rank = 0;
            return false;
        }
    }

    *rank = score;
    return true;
}

bool ScopeSelector::match(const Scope& scope, char side, double* rank) const {
    double specific_core = 0, atom_score = 0, depth_score = 0;
    double tmp_score;

    if( match_atom_prefix(atoms.back(), scope.back(), &tmp_score)) {
        specific_core = tmp_score;
    } else {
        if (anchor_end) {
            *rank = 0;
            return false;
        }
    }
    if (anchor_begin && !match_atom_prefix(atoms.front(), scope.front(), &tmp_score)) {
        *rank = 0;
        return false;
    }

    // greedy algorithm
    size_t it_sel = 0;
    for (size_t it_scope = 0; it_scope < scope.size(); it_scope++) {
        if (it_sel < atoms.size() && match_atom_prefix(atoms[it_sel], scope[it_scope], &tmp_score)) {
            it_sel++;
            atom_score += tmp_score;
        }
    }

    if (it_sel >= atoms.size()) {
        depth_score = atoms.size();
    } else {
        *rank = 0;
        return false;
    }


    *rank = specific_core * 1000 + atom_score * 100 + depth_score;
    return true;
}

bool ScopeNameSelector::match(const Scope& scope, char side, double* rank) const {
    throw ShlException("match not implemented for ScopeNameSelector");
}

}
}
