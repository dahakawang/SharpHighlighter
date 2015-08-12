#ifndef _TOKENIZER_H_
#define _TOKENIZER_H_

#include <utility>
#include <stack>
#include "grammar.h"
#include "regex.h"
#include "scope.h"

using std::pair;
using std::stack;

namespace shl {

class Tokenizer {
public:
    enum Option {OPTION_NONE = 0, OPTION_TOLERATE_ERROR = 1, OPTION_STRICT = 2};

    Tokenizer():_option(OPTION_NONE) {};
    Tokenizer(Option option):_option(option) {};
    vector<pair<Range, Scope> > tokenize(const Grammar& grammar, const string& text); 

private:
    Option _option;
    Match tokenize(const string& text, const Rule& rule, const Match& begin_lexeme, vector<const Rule*>& stack, vector<pair<Range, Scope> >& tokens);
    bool next_lexeme(const string& text, const int begin_end_pos, const Match& begin_lexeme, const Rule& rule, const Rule** found, Match& match);
    void add_scope(vector<pair<Range, Scope> >& tokens, const Range& range, vector<const Rule*>& stack, const string& name, const string& enclosing_name);
    void process_capture(vector<pair<Range, Scope> >& tokens, const Match& match, vector<const Rule*>& stack, const map<int, string>& capture, const string& enclosing_name);
};

}
#endif
