#include <vector>
#include <utility>
#include <algorithm>
#include "tokenizer.h"
#include "shl_exception.h"

using std::stack;

namespace shl {

vector<pair<Range, Scope> > Tokenizer::tokenize(const Grammar& grammar, const string& text) {
    vector<pair<Range, Scope> > tokens;
    vector<const Pattern*> pattern_stack;

    tokens.push_back(std::make_pair(Range(0, text.size()), Scope(grammar.name)));
    tokenize(text, grammar, Match::make_dummy(0,0), pattern_stack, tokens);

    return tokens;
}

/**
 * Find the next lexeme iteratively
 *
 * This method find the next lexeme by matching all begin field of the sub-patterns
 * of current rule, and its own end field, whichever comes first will be seleteced
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
    for (const Pattern& pattern : pattern.patterns) {
        const Pattern& sub_pattern = (pattern.include == nullptr)? pattern : *pattern.include;

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

vector<string> get_name(vector<const Pattern*>& stack, const string& name) {
    vector<string> names;

    for(auto pattern : stack) {
        names.push_back(pattern->name);
    }
    names.push_back(name);

    return names;
}

void Tokenizer::add_scope(vector<pair<Range, Scope> >& tokens, const Range& range, vector<const Pattern*>& stack, const string& name) {
    if (name.empty()) return;

    Scope scope(get_name(stack, name));
    tokens.push_back(std::make_pair(range, scope));
}

inline void append_back(vector<pair<Range, Scope> >& target, const vector<pair<Range, Scope> >& source ) {
    std::move(source.begin(), source.end(), std::back_inserter(target));
}

void Tokenizer::process_capture(vector<pair<Range, Scope> >& tokens, const Match& match, vector<const Pattern*>& stack, const map<int, string>& capture) {
    for (auto& pair : capture) {
        unsigned int capture_num = pair.first;
        const string& name = pair.second;
        if (match.size() > capture_num) {
            add_scope(tokens, match[capture_num], stack, name);
        } else {
            if (_option & OPTION_TOLERATE_ERROR == 0) {
                throw InvalidSourceException("capture number out of range");
            }
        }
    }
}

Match Tokenizer::tokenize(const string& text, const Pattern& pattern, const Match& begin_lexeme, vector<const Pattern*>& stack, vector<pair<Range, Scope> >& tokens) {
    stack.push_back(&pattern);

    const Pattern* found_pattern;
    Match last_lexeme, match;
    last_lexeme = begin_lexeme;
    while(next_lexeme(text, begin_lexeme, pattern, &found_pattern, match)) {
        if (found_pattern->is_match_rule) {
            add_scope(tokens, match[0], stack, found_pattern->name);
            process_capture(tokens, match, stack, found_pattern->captures);

        } else {
            vector<pair<Range, Scope> > child_tokens;

            Match end_match = tokenize(text, *found_pattern, match, stack, child_tokens);
            
            Range name_range = Range(match[0].position, end_match[0].end() - match[0].position);
            add_scope(tokens, name_range, stack, "");
            process_capture(tokens, match, stack, found_pattern->begin_captures);

            Range content_range = Range(match[0].end(), end_match[0].position - match[0].end());
            add_scope(tokens, content_range, stack, found_pattern->content_name);
            
            append_back(tokens, child_tokens);
            process_capture(tokens, match, stack, found_pattern->end_captures);
            
        }

        last_lexeme = match;
    }

    stack.pop_back();

    if ( found_pattern == nullptr) { //see comments for next_lexeme
        return last_lexeme; 
    } else {
        return match; 
    }

}

}
