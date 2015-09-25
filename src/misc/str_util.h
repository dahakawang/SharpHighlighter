#ifndef __STR_UTIL_H__
#define __STR_UTIL_H__

#include <string>
#include <vector>

using std::string;
using std::vector;

namespace shl {

template<typename T>
vector<string> strtok(const string& str, T predicator) {
    const char* s = str.c_str();
    auto length = str.size();
    vector<string> tokens;

    bool in_separator = true;
    string::size_type start, index;
    for(index = 0; index < length; index++) {
        if (in_separator) {
            if (!predicator(s[index])) {
                in_separator = false;
                start = index;
            }
        } else {
            if (predicator(s[index])) {
                in_separator = true;
                tokens.push_back(str.substr(start, index - start));
            }
        }
    }
    if (!in_separator) {
        tokens.push_back(str.substr(start));
    }

    return tokens;
}

string trim(const string& str);

}

#endif
