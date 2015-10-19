#include "selector.h"
#include <string>

using std::string;

namespace shl {
namespace selector {

string ScopeNameSelector::str() const {
    string buffer;
    buffer += components[0];

    for (size_t pos = 1; pos < components.size(); ++pos) {
        buffer += '.';
        buffer += components[pos];
    }

    return buffer;
}

string ScopeSelector::str() const {
    string buffer;

    if (anchor_begin) buffer += "^ ";
    buffer += atoms[0].str();

    for (size_t pos = 1; pos < atoms.size(); ++pos) {
        if (atoms[pos].anchor_prev) buffer += " > ";
        else buffer += " ";
        buffer += atoms[pos].str();
    }

    if (anchor_end) buffer += " $";

    return buffer;
}

string GroupSelector::str() const {
    return "(" + selector.str() + ")";
}

string FilterSelector::str() const {
    string buffer;
    if (side != FilterSelector::None) {
        buffer += side;
        buffer += ": ";
    }
    buffer += selector->str();

    return buffer;
}

string ExpressionSelector::str() const {
    string buffer;
    if (is_negative) buffer += "- ";
    buffer += selector.str();

    return buffer;
}

string CompositeSelctor::str() const {
    string buffer;
    buffer += selectors[0].str();

    for(size_t pos = 1; pos < selectors.size(); ++pos) {
        buffer += ' ';
        buffer += operators[pos];
        buffer += ' ';
        buffer += selectors[pos].str();
    }

    return buffer;
}

string Selector::str() const {
    string buffer;
    for(size_t pos = 0; pos < selectors.size(); ++pos) {
        if (pos != 0) buffer += ", ";
        buffer += selectors[pos].str();
    }

    return buffer;
}

}
}
