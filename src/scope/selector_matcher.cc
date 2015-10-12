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

bool match(const ScopeNameSelector& selector, const ScopeName& atom, double* rank) {
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
    using shl::selector::match;
    double specific_core = 0, atom_score = 0, depth_score = 0;
    double tmp_score;

    // greedy algorithm
    vector<int> matched_pos(atoms.size(), 0);
    size_t it_sel = 0, it_scope = 0;
    while(it_sel < atoms.size() && it_scope < scope.size()) {
        bool backtrace = false;
        for(; it_scope < scope.size() && !match(atoms[it_sel], scope[it_scope], &tmp_score); it_scope++);
        if (it_scope < scope.size()) { //matched
            if (anchor_begin && it_sel == 0 && it_scope != 0) break;
            if (anchor_end && it_sel == atoms.size() - 1 && it_scope != scope.size() - 1) backtrace = true;
            if (atoms[it_sel].anchor_prev && it_sel > 0 && it_scope != matched_pos[it_sel - 1] + 1) backtrace = true;
        } else {
            break;
        }
        if (backtrace) {
            if (it_sel != 0) {
                it_sel--;
                it_scope = matched_pos[it_sel] + 1;
            } else {
                it_scope++;
            }
        } else {
            matched_pos[it_sel] = it_scope;
            it_sel++;
            it_scope++;
        }
    }

    if (it_sel >= atoms.size()) {
        depth_score = atoms.size();
    } else {
        if (rank) *rank = 0;
        return false;
    }


    if (rank) *rank = specific_core * 1000 + atom_score * 100 + depth_score;
    return true;
}

bool ScopeNameSelector::match(const Scope& scope, char side, double* rank) const {
    throw ShlException("match not implemented for ScopeNameSelector");
}

}
}
