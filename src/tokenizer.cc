#include <stack>
#include "tokenizer.h"
#include "shl_exception.h"

using std::stack;

namespace shl {

vector<pair<Range, Scope> > Tokenizer::tokenize(const Grammar& grammar, const string& text) {
    vector<pair<Range, Scope> > tokens;
    stack<const Pattern*> pattern_stack;

    tokens.push_back(std::make_pair(Range(0, text.size()), Scope(grammar.name)));
    tokenize(text, grammar, Match::make_dummy(0,0), pattern_stack, tokens);

    return tokens;
}

/**
 * Find the next lexeme iteratively
 *
 * This method find the next lexeme by matching all begin field of its sub-patterns
 * as well as its own end field, whichever comes first will be seleteced
 *
 * When true returned, found will contain the selected next pattern, and match will hold 
 * the results. When false returned, there are 3 scenarios:
 * 1. end field is matched, found & match contains results
 * 2. current pattern is toplevel rule, so no end field, found is nullptr
 * 3. When nothing can be matched(either source text or syntax definition is invalid),
 *    and user specified OPTION_TOLERATE_ERROR, found will be nullptr, otherwise exception
 *    will be thrown
 */
bool Tokenizer::next_lexeme(const string& text, const Match& begin_lexeme, const Pattern& pattern, const Pattern** found, Match& match) {
    int pos = begin_lexeme[0].end();
    const Pattern* found_pattern;
    Match first_match;
    bool is_close = false;

    // first find pattern or end pattern, whichever comes first
    for (const Pattern& sub_pattern : pattern.patterns) {
        Match tmp = sub_pattern.begin.match(text, pos);
        if (tmp != Match::NOT_MATCHED) {
            if( found_pattern == nullptr || tmp[0].position < first_match[0].position) {
                first_match = std::move(tmp); 
                found_pattern = &sub_pattern;
            }
        }
    }
    if (!pattern.end.empty()) {
        Match tmp = pattern.end.match(text, pos);
        if( found_pattern != nullptr || tmp[0].position < first_match[0].position) {
            first_match = std::move(tmp); 
            found_pattern = &pattern;
            is_close = true;
        }
    }
    if ( found_pattern != nullptr) {
        *found = found_pattern;
        match = first_match;
        return is_close;
    }

    // special handle for toplevel rule
    if (pattern.end.empty()) {
        // all rule with begin will has an end, which is enforced by grammar loader
        // so only toplevel rules can be here
        *found = nullptr;
        is_close = true;
        return false;
    }

    // When no lexeme found
    if (_option & OPTION_TOLERATE_ERROR) {
        *found = nullptr;
        return false;
    } else {
       throw InvalidSourceException("scope not properly closed"); 
    }
}

Match Tokenizer::tokenize(const string& text, const Pattern& pattern, const Match& begin_lexeme, stack<const Pattern*>& stack, vector<pair<Range, Scope> >& tokens) {
    stack.push(&pattern);

    const Pattern* found_pattern;
    Match last_lexeme, match;
    last_lexeme = begin_lexeme;
    while(next_lexeme(text, begin_lexeme, pattern, &found_pattern, match)) {

        last_lexeme = match;
    }


    stack.pop();

    return last_lexeme;
}

}
