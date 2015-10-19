#ifndef __SELECTOR_PARSER__
#define __SELECTOR_PARSER__

#include <memory>
#include <selector.h>

using std::shared_ptr;

namespace shl {
namespace selector {

class Parser {
public:
    Parser(const string& str);
    Parser(const char* str);
    Selector parse();

private:
    string _selector_str;
    size_t _pos;

    bool ws();
    bool parse_char(const char* char_set, char* found_char = nullptr);
    bool parse_selector(Selector& selector);
    bool parse_composite(CompositeSelctor& selector);
    bool parse_expression(ExpressionSelector& selector);
    bool parse_filter(FilterSelector& selector);
    bool parse_group(shared_ptr<AbstractSelector>& selector);
    bool parse_scope(shared_ptr<AbstractSelector>& selector);
    bool parse_scope_name(ScopeNameSelector& selector, bool is_first);

};

}
}


#endif
