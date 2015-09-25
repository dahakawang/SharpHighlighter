#include <string.h>
#include <shl_exception.h>
#include "selector.h"

namespace shl {
namespace selector {

/*
    atom:         «string» | '*'
    scope:        «atom» ('.' «atom»)*
    path:         '^'? «scope» ('>'? «scope»)* '$'?
    group:        '(' «selector» ')'
    filter:       ("L:"|"R:"|"B:") («group» | «path»)
    expression:   '-'? («filter» | «group» | «path»)
    composite:    «expression» ([|&-] «expression»)*
    selector:     «composite» (',' «composite»)*
*/

bool ws(const string& str, size_t& pos) {
    while(isblank(str[pos])) pos++;

    return true;
}

bool parse_char(const string& str, size_t& pos, const char* chars, char* op = nullptr) {
    if(pos >= str.size() || !strchr(chars, str[pos])) {
        return false;
    }
    if (op) *op = str[pos];
    ++pos;
    return true;
}

void ensure_not_empty(const AbstractSelector& obj) {
    if (obj.empty()) {
        throw InvalidScopeSelector("failed to parse selector");
    }
}

Selector Selector::parse(const string& str, size_t& pos) {
    Selector instance;

    do {
        ws(str, pos);
        CompositeSelctor tmp = CompositeSelctor::parse(str, pos);
        ensure_not_empty(tmp);
        instance.selectors.push_back(std::move(tmp));
        ws(str, pos);
    } while(parse_char(str, pos, ","));

    return instance;
}

CompositeSelctor CompositeSelctor::parse(const string& str, size_t& pos) {
    CompositeSelctor instance;
    char op = 0;

    do {
        ws(str, pos);
        instance.operators.push_back((CompositionType)op);
        ExpressionSelector tmp = ExpressionSelector::parse(str, pos);
        ensure_not_empty(tmp);
        instance.selectors.push_back(std::move(tmp));
        ws(str, pos);
    } while(parse_char(str, pos, "&|-", &op));

    return instance;
}


ExpressionSelector ExpressionSelector::parse(const string& str, size_t& pos) {
    ExpressionSelector instance;

    if(parse_char(str, pos, "-")) instance.is_negative = true;
    ws(str, pos);
}

}
}
