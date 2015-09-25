#include <string>
#include "str_util.h"

using std::string;

namespace shl {

string trim(const string& str) {
    const char* s = str.c_str();
    string::size_type start, end;
    start = 0;
    end = str.size();

    while(start < end && isblank(s[start])) start++;
    while(end > start && isblank(s[end-1])) end--;

    return str.substr(start, end - start);
}

}
