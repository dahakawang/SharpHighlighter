#include <memory>
#include <shl_exception.h>
#include "selector_parser.h"

using std::unique_ptr;

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

Parser::Parser(const string& str):_selector_str(str), _pos(0) {
}

Parser::Parser(const char* str):_selector_str(str), _pos(0) {
}

bool Parser::ws() {
    while(_pos < _selector_str.size() && isblank(_selector_str[_pos])) ++_pos;

    return true;
}

bool Parser::parse_char(const char* char_set, char* found_char) {
    if (_pos >= _selector_str.size() || !strchr(char_set, _selector_str[_pos])) {
        return false;
    }

    if (found_char) *found_char = _selector_str[_pos];
    ++_pos;
    return true;
}

bool Parser::parse_selector(Selector& selector) {
    do {
        ws();
        CompositeSelctor tmp;
        if (!parse_composite(tmp)) return false;
        selector.selectors.push_back(std::move(tmp));
        ws();
    } while(parse_char(","));

    return true;
}

bool Parser::parse_composite(CompositeSelctor& selector) {
    char op = 0;

    do {
        ws();
        selector.operators.push_back((CompositeSelctor::CompositionType)op);
        ExpressionSelector tmp;
        if (parse_expression(tmp)) return false;
        selector.selectors.push_back(std::move(tmp));
        ws();
    } while (parse_char("&|-", &op));

    return true;
}

bool Parser::parse_expression(ExpressionSelector& selector) {
    ws();
    if (parse_char("-")) selector.is_negative = true;
    ws();

    return parse_filter(selector.selector) || parse_group(selector.selector) || parse_scope(selector.selector);

}

bool Parser::parse_filter(unique_ptr<AbstractSelector>& selector) {
    char side;
    size_t saved_pos = _pos;

    ws();
    unique_ptr<FilterSelector> filter(new FilterSelector());
    if (parse_char("LRB", &side) && parse_char(":") && ws() && (parse_group(filter->selector) || parse_scope(filter->selector))) {
        filter->side = (FilterSelector::Side)side;
        selector = std::move(filter);
        return true;
    }

    _pos = saved_pos;
    return false;
}


bool Parser::parse_group(unique_ptr<AbstractSelector>& selector) {
    size_t saved_pos = _pos;

    ws();
    unique_ptr<GroupSelector> group(new GroupSelector());
    if (parse_char("(") && parse_selector(group->selector) && ws() && parse_char(")")) {
        selector = std::move(group);
        return true;
    }

    _pos = saved_pos;
    return false;
}

bool Parser::parse_scope(unique_ptr<AbstractSelector>& selector) {
    size_t saved_pos = _pos;

    ws();
    unique_ptr<ScopeSelector> scope(new ScopeSelector());
    scope->anchor_begin = parse_char("^");
    bool first_atom = true;
    while(ws()) {
        ScopeNameSelector scope_name;
        if (!parse_scope_name(scope_name, first_atom)) break;
        scope->atoms.push_back(std::move(scope_name));
        first_atom = false;
    }
    scope->anchor_end = parse_char("$");

    if (scope->atoms.empty()) {
        _pos = saved_pos;
        return false;
    } else {
        selector = std::move(scope);
        return true;
    }
}

bool Parser::parse_scope_name(ScopeNameSelector& selector, bool is_first) {
    ws();
    if (is_first) selector.anchor_prev = parse_char(">");
    ws();
    while(_pos < _selector_str.size()) {
        int start = _pos;
        while(_pos < _selector_str.size() && (isalnum(_selector_str[_pos]) || _selector_str[_pos] == '*')) ++_pos;
        
        if (_pos >= _selector_str.size() || isblank(_selector_str[_pos])) {
            if (_pos - start <= 0) throw InvalidScopeSelector("0 length scope component");
            selector.components.push_back(_selector_str.substr(start, _pos - start));
            break;
        } else if (_selector_str[_pos] == '.') {
            if (_pos - start <= 0) throw InvalidScopeSelector("0 length scope component");
            selector.components.push_back(_selector_str.substr(start, _pos - start));
        } else {
            throw InvalidScopeSelector("invalid charactor");
        }
    }

    return true;
}

}
}
