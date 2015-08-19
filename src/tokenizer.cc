#include <vector>
#include <utility>
#include <algorithm>
#include <functional>
#include <unordered_set>
#include <set>
#include "tokenizer.h"
#include "shl_exception.h"

using std::stack;
using std::function;
using std::unordered_set;
using std::set;

namespace shl {

vector<pair<Range, Scope> > Tokenizer::tokenize(const Grammar& grammar, const string& text) {
    vector<pair<Range, Scope> > tokens;
    vector<const Rule*> rule_stack;

    tokens.push_back(std::make_pair(Range(0, text.size()), Scope(grammar.name)));
    tokenize(text, grammar, Match::make_dummy(0,0), rule_stack, tokens);

    return tokens;
}

static const Rule& resolve_include(const Rule& rule, vector<const Rule*>& stack) {
    if ( rule.include.is_base_ref ) {
        return *stack[0];
    } else if (rule.include.ptr) {
        return *rule.include.ptr;
    } else {
        return rule;
    }
}

static void for_all_subrules(const Rule& rule, set<const Rule*>& visited, vector<string>& scope_names, vector<const Rule*>& stack, function<void(const Rule&, const vector<string>&)> callback) {
    const Rule& real_rule = resolve_include(rule, stack);
    if (visited.count(&real_rule) > 0) return;
    visited.insert(&real_rule);
    scope_names.push_back(real_rule.name);

    if (real_rule.begin.empty()) {
        for (auto& subrule : real_rule.patterns) {
            scope_names.push_back(real_rule.content_name);
            for_all_subrules(subrule, visited, scope_names, stack, callback);
            scope_names.pop_back();
        }
    } else {
        // we don't need to look at it's subrule at this time
        callback(real_rule, scope_names);
    }

    scope_names.pop_back();
}

static void for_all_subrules(const vector<Rule>& rules, vector<const Rule*>& stack, function<void(const Rule&, const vector<string>&)> callback) {
    set<const Rule*> visited;
    vector<string> scope_names;
    for (auto& rule : rules) {
        for_all_subrules(rule, visited, scope_names, stack, callback);
    }
}

static inline bool is_end_match_first(const Rule& rule, const Match& end_match, const Match& current_first_match) {
    if (rule.applyEndPatternLast) {
        return end_match[0].position < current_first_match[0].position;
    } else {
        return end_match[0].position <= current_first_match[0].position;
    }
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
 *
 * Note:
 * begin_end_pos: this is the offset of current pattern's begin match.end(), it's meant to 
 * support Perl style \G, and stands for the *previous* match.end()
 */
bool Tokenizer::next_lexeme(const string& text, const Match& begin_lexeme, const Match& last_lexeme, const Rule& rule, const Rule** found, Match& match, vector<string>& scope_names, vector<const Rule*>& stack) {
    int begin_end_pos = begin_lexeme[0].end();
    int pos = last_lexeme[0].end();
    const Rule* found_rule = nullptr;
    Match first_match;
    vector<string> found_scope_names;
    bool is_close = false;

    // first find pattern or end pattern, whichever comes first
    for_all_subrules(rule.patterns, stack, [&found_scope_names, &found_rule, &first_match, pos, &text, begin_end_pos](const Rule& sub_rule, const vector<string>& cur_scope_names) {
        Match tmp = sub_rule.begin.match(text, pos, begin_end_pos);
        if (tmp != Match::NOT_MATCHED) {
            if( found_rule == nullptr || tmp[0].position < first_match[0].position) {
                first_match = std::move(tmp); 
                found_scope_names = cur_scope_names;
                found_rule = &sub_rule;
            }
        }
    });
    if (!rule.end.empty()) {
        Match end_match = rule.end.match(begin_lexeme, text, pos, begin_end_pos); 
        if (end_match != Match::NOT_MATCHED) {
            if( found_rule == nullptr || is_end_match_first(rule, end_match, first_match)) {
                first_match = std::move(end_match); 
                found_scope_names.clear();
                found_rule= &rule;
                is_close = true;
            }
        }
    }
    if ( found_rule != nullptr) {
        *found = found_rule;
        match = std::move(first_match);
        scope_names = std::move(found_scope_names);
        return !is_close;
    }

    // special handle for toplevel rule
    if (stack[0] == &rule) {
        *found = nullptr;
        return false;
    }

    // When no lexeme found
    if (_option & OPTION_TOLERATE_ERROR) {
        *found = nullptr;
        return false;
    } else {
       throw InvalidSourceException("scope not properly closed: " + rule.name); 
    }
}

vector<string> compile_scope_names(vector<const Rule*>& stack, const vector<string> scope_names) {
    vector<string> names;

    for(auto rule: stack) {
        names.push_back(rule->name);
        names.push_back(rule->content_name);
    }
    for(auto elem : scope_names) {
        names.push_back(elem);
    }

    return names;
}

void Tokenizer::add_scope(vector<pair<Range, Scope> >& tokens, const Range& range, vector<const Rule*>& stack, const vector<string>& scope_names) {
    if (scope_names.back().empty()) return;
    if (range.length == 0) return;

    Scope scope(compile_scope_names(stack, scope_names));
    tokens.push_back(std::make_pair(range, scope));
}

void Tokenizer::add_scope(vector<pair<Range, Scope> >& tokens, const Range& range, vector<const Rule*>& stack, vector<string>& scope_names, const string& name) {
    scope_names.push_back(name);
    add_scope(tokens, range, stack, scope_names);
    scope_names.pop_back();
}

inline void append_back(vector<pair<Range, Scope> >& target, const vector<pair<Range, Scope> >& source ) {
    std::move(source.begin(), source.end(), std::back_inserter(target));
}

vector<string> get_parent_capture_names(const vector<string> scope_names, const Match& match, const map<int, string>& capture, size_t pos) {
    vector<string> result(scope_names);

    for (size_t it = 0; it < pos; it++) {
        auto it_name = capture.find(it);
        if (match[it].contain(match[pos]) && it_name != capture.end()) {
            result.push_back(it_name->second);
        }
    }

    return result;
}

void Tokenizer::process_capture(vector<pair<Range, Scope> >& tokens, const Match& match, vector<const Rule*>& stack, const map<int, string>& capture, const vector<string>& scope_names) {
    for (auto& pair : capture) {
        unsigned int capture_num = pair.first;
        const string& name = pair.second;
        if (match.size() > capture_num) {
            if (match[0].contain(match[capture_num])) {
                auto merged_scope_names = get_parent_capture_names(scope_names, match, capture, capture_num);
                add_scope(tokens, match[capture_num], stack, merged_scope_names, name);
            }
        } else {
            if (_option & OPTION_STRICT) {
                throw InvalidSourceException("capture number out of range");
            }
        }
    }
}

// when we can't find the end, last found lexeme will be returned
// But when current rule even dose not contain any sub-rule, then
// the last found lexeme will be the begin lexeme of current rule
// In this case we should advance the parser pointer by 1 to avoid infinite loop
inline static bool detect_infinite_loop(const Match& begin, Match& end) {
    return (begin[0].end() >= end[0].end());
}

Match Tokenizer::tokenize(const string& text, const Rule& rule, const Match& begin_lexeme, vector<const Rule*>& stack, vector<pair<Range, Scope> >& tokens) {
    stack.push_back(&rule);

    const Rule* found_rule = nullptr;
    Match last_lexeme, match;
    last_lexeme = begin_lexeme;
    vector<string> scope_names;
    while(next_lexeme(text, begin_lexeme, last_lexeme, rule, &found_rule, match, scope_names, stack)) {
        if (found_rule->is_match_rule) {
            add_scope(tokens, match[0], stack, scope_names);
            process_capture(tokens, match, stack, found_rule->begin_captures, scope_names);
            last_lexeme = match;

        } else {
            vector<pair<Range, Scope> > child_tokens;

            Match end_match = tokenize(text, *found_rule, match, stack, child_tokens);
            if (detect_infinite_loop(match, end_match)) {
                last_lexeme = end_match;
                last_lexeme[0].length++;

            } else {
                Range name_range = Range(match[0].position, end_match[0].end() - match[0].position);
                add_scope(tokens, name_range, stack, scope_names);
                process_capture(tokens, match, stack, found_rule->begin_captures, scope_names);

                Range content_range = Range(match[0].end(), end_match[0].position - match[0].end());
                add_scope(tokens, content_range, stack, scope_names, found_rule->content_name);

                append_back(tokens, child_tokens);
                process_capture(tokens, end_match, stack, found_rule->end_captures, scope_names);

                last_lexeme = end_match;
            }
        }
    }

    stack.pop_back();

    if ( found_rule == nullptr) { //see comments for next_lexeme
        return last_lexeme; 
    } else {
        return match; 
    }

}

}
