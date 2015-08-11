#include "structured_scope.h"
#include "shl_exception.h"

namespace shl {

string StructuredScope::to_markup(const string& text, const vector<pair<Range, Scope> >& scopes) {
    if (scopes.size() <= 0) return text;

    string str;
    build(text, scopes, 0, 0, str);

    return str;
}

string join(const vector<string>& name, int first, int last) {
    string res(name[first]);
    for(int i = first + 1; i <= last; i++) {
        res.append(".");
        res.append(name[i]);
    }
    return res;
}

string markup_class(const vector<string>& name) {
    string clazz;

    for (unsigned i = 0; i < name.size(); i++) {
        clazz.append(name[i]);
        clazz.append(" ");
    }

    return clazz;
}

inline void put_open_tags(Scope& name_scope, string& target) {
    for (unsigned i = 0; i < name_scope.size(); i++) {
        target.append("<span class=\"" + markup_class(name_scope[i].breakdown()) + "\">");
    }
}

inline void put_close_tags(Scope& name_scope, string& target) {
    for (int i = name_scope.size() - 1; i >= 0; i--) {
        target.append("</span>");
    }
}

inline void put_text(const string& text, unsigned start, unsigned length, string& target) {
    target.append(text.substr(start, length));
}

unsigned StructuredScope::build(const string& text, const vector<pair<Range, Scope> >& scopes, unsigned pos, unsigned depth, string& markup) {
    const Scope& current_scope = scopes[pos].second;
    Range current_range = scopes[pos].first;
    Scope name_scope = current_scope.subscope(depth);

    put_open_tags(name_scope, markup);


    unsigned last_end = current_range.position;
    pos++;
    while(pos < scopes.size()) {
        const Scope& next_scope = scopes[pos].second;
        Range next_range = scopes[pos].first;

        if (current_scope != next_scope && current_scope.is_prefix_of(next_scope)) {
            put_text(text, last_end, next_range.position - last_end, markup);

            last_end = scopes[pos].first.end(); // pos is the root of next subtree
            pos = build(text, scopes, pos,  depth + 1, markup);
        } else {
            break;
        }
    }
    put_text(text, last_end, current_range.end() - last_end, markup);

    put_close_tags(name_scope, markup);
    return pos;
}

}
