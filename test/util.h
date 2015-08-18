#ifndef __UTIL_H__
#define __UTIL_H__

#include <string>
#include <vector>
#include <grammar_registry.h>

using std::string;
using std::vector;

const string load_string(const string& file_path);

class TestGrammarSource : public shl::GrammarSource {
public:
    TestGrammarSource(const vector<string>& mapping);
    virtual const std::string load_data(const std::string& grammar_name) const;
    virtual const std::vector<std::string> list_grammars() const;

private:
    map<string, string> _mapping;
};

#endif
