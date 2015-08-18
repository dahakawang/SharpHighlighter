#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "util.h"
#include <sstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <iterator>
#include <stdexcept>

using std::ostringstream;
using std::string;
using std::ifstream;
using std::copy;
using std::istreambuf_iterator;
using std::ostreambuf_iterator;


const string load_string(const string& file_path) {
    string path = "test/" + file_path;
    ostringstream stream;
    ifstream fs(path);

    if (!fs.good()) {
        throw std::runtime_error("error access file " + path);
    }

    copy(istreambuf_iterator<char>(fs), istreambuf_iterator<char>(), ostreambuf_iterator<char>(stream));
    
    return stream.str();
}

TestGrammarSource::TestGrammarSource(const vector<string>& mapping) {
    if (mapping.size() % 2 != 0) throw std::runtime_error("the mapping should be in pair");

    for(size_t pos = 0; pos < mapping.size(); pos += 2) {
        _mapping[mapping[pos]] = mapping[pos+1];
    }
}
const std::string TestGrammarSource::load_data(const std::string& grammar_name) const {
    if (_mapping.count(grammar_name) == 0) return "";
    return load_string(_mapping.find(grammar_name)->second);
}
const std::vector<std::string> TestGrammarSource::list_grammars() const {
    vector<string> result;

    for ( auto& pair : _mapping ) {
        result.push_back(pair.first);
    }

    return result;
}
