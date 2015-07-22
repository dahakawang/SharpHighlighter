#include <cctype>
#include <string>
#include <vector>
#include <sstream>
#include <cctype>
#include "scope.h"

using std::string;
using std::vector;
using std::ostringstream;

namespace shl {

vector<string> strtok(const string& str) {
    const char* s = str.c_str();
    auto length = str.size();
    vector<string> tokens;

    bool in_blank = true;
    string::size_type start, index;
    for(index = 0; index < length; index++) {
        if (in_blank) {
            if (!isblank(s[index])) {
                in_blank = false;
                start = index;
            }
        } else {
            if (isblank(s[index])) {
                in_blank = true;
                tokens.push_back(str.substr(start, index - start));
            }
        }
    }
    if (!in_blank) {
        tokens.push_back(str.substr(start));
    }

    return tokens;
}
    
Scope::Scope(const string& scope) {
    _scope = strtok(scope);
}

string trim(const string& str) {
    const char* s = str.c_str();
    string::size_type start, end;
    start = 0;
    end = str.size();

    while(start < end && isblank(s[start])) start++;
    while(end > start && isblank(s[end-1])) end--;

    return str.substr(start, end - start);
}

Scope::Scope(const vector<string>& scope) {
    for (auto& name : scope) {
        if(!name.empty()) _scope.push_back(name);
    }
}

string Scope::name() const {
    ostringstream name;
    bool first = true;
    for (auto& component : _scope) {
        if (first) {
            first = false;
        } else {
            name << ' ';
        }
        name << component;
    }

    return name.str();
}

}
