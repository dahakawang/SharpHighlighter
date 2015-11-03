#ifndef __SCOPE_MACRO_H__
#define __SCOPE_MACRO_H__

#include <string>
#include <shl_regex.h>

using std::string;

namespace shl {

string expand_macro(const string& token, const string& text, const Match& match);

}

#endif
