#include <stack>
#include "tokenizer.h"
#include "shl_exception.h"

using std::stack;

namespace shl {

vector<pair<Range, Scope> > Tokenizer::tokenize(const Grammar& grammar, const string& text) {
    vector<pair<Range, Scope> > tokens;
    stack<const Pattern*> pattern_stack;

    tokens.push_back(std::make_pair(Range(0, text.size()), Scope(grammar.name)));
    tokenize(text, grammar, Match(), pattern_stack, tokens);

    return tokens;
}

bool Tokenizer::next_lexeme(const string& text, const Match& begin_lexeme, const Pattern& pattern, const Pattern* parent, const Pattern** found, Match& match) {
    int pos = (begin_lexeme == Match::NOT_MATCHED) ? 0 : begin_lexeme[0].end();
    Match first_match;
    const Pattern* found_pattern;
    bool is_close = false;

    for (const Pattern& sub_pattern : pattern.patterns) {
        Match tmp = sub_pattern.begin.match(text, pos);
        if (tmp != Match::NOT_MATCHED) {
            if( first_match == Match::NOT_MATCHED || tmp[0].position < first_match[0].position) {
                first_match = std::move(tmp); //! tmp never used afterwards
                found_pattern = &sub_pattern;
            }
        }
    }
    if (!pattern.end.empty()) {
        Match tmp = pattern.end.match(text, pos);
        if( first_match == Match::NOT_MATCHED || tmp[0].position < first_match[0].position) {
            first_match = std::move(tmp); //! tmp never used afterwards
            found_pattern = &pattern;
            is_close = true;
        }
    } else {
        if (found_pattern == nullptr) {
            is_close = true;

        }
    }

    if ( found_pattern != nullptr) {
        *found = found_pattern;
        match = first_match;
        return is_close;
    }



    // When no lexeme found
    if (_option & OPTION_TOLERATE_ERROR == 0) {
       throw InvalidSourceException("scope not properly closed"); 
    } else {
        *found = nullptr;
        return false;
    }
}

void Tokenizer::tokenize(const string& text, const Pattern& pattern, const Match& begin_lexeme, stack<const Pattern*>& stack, vector<pair<Range, Scope> >& tokens) {
    const Pattern* parent = stack.empty() ? nullptr : stack.top();
    stack.push(&pattern);

    const Pattern* found_pattern;
    Match match;
    while(next_lexeme(text, begin_lexeme, pattern, parent, &found_pattern, match)) {
        
    }


    stack.pop();
}

}
