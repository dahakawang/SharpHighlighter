#include <locale>
#include <scope_macro.h>
#include <str_util.h>
#include <shl_exception.h>

namespace shl {

string expand_macro(const string& token, const string& text, const Match& match) {
    if (token[0] != '$') return token;

    if (token[1] == '{') {
        if (token.back() != '}') throw InvalidSourceException("scope macro not properly closed");
        string num_command = token.substr(2, token.size() - 3);
        vector<string> num_command_pair = strtok(num_command, [](char c) { return c == ':';});

        int group_num = atoi(num_command_pair[0].c_str());
        if (group_num >= match.size()) throw InvalidSourceException("capture group number in scope name out of range");

        string expanded = match[group_num].substr(text);

        std::locale loc;
        if (num_command_pair[1] == "/downcase") {
            return tolower(expanded, loc);
        } else if (num_command_pair[1] == "/upcase") {
            return std::toupper(expanded, loc);
        } else {
            throw InvalidSourceException("unknown expansion function " + num_command_pair[1]);
        }

    } else {
        int group_num = atoi(token.c_str() + 1);
        if (group_num >= match.size()) throw InvalidSourceException("capture group number in scope name out of range");
        return match[group_num].substr(text);
    }
}

}
