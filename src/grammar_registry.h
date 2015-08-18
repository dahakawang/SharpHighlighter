#ifndef __GRAMMAR_REGISTRY_H__
#define __GRAMMAR_REGISTRY_H__

#include "grammar.h"
#include <string>
#include <map>
#include <vector>

namespace shl {

class GrammarSource {
public:
    virtual const std::string load_data(const std::string& grammar_name) const = 0;
    virtual const std::vector<std::string> list_grammars() const = 0;
};

class GrammarRegistry {
public:
    GrammarRegistry(const std::shared_ptr<GrammarSource> data_source);
    bool contain_grammar(const std::string& grammar_name) const;
    const Grammar& get_grammar(const std::string& grammar_name) const;
    Grammar& load_grammar(const std::string& grammar_name);

private:
    std::map<std::string, Grammar> _grammars;
    std::shared_ptr<GrammarSource> _source;
    Grammar _empty;

    Grammar& add_grammar(const std::string& data);
};

}

#endif
