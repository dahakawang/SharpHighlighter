#include <iostream>
#include <memory>
#include "regex.h"
#include "shl_exception.h"

using std::shared_ptr;

namespace shl {

inline string onig_error_string(int code, OnigErrorInfo* error_info, const string& regex) {
    UChar buffer[1024];
    int length = onig_error_code_to_str(buffer, code, error_info);

    return string(buffer, buffer + length) + " -> " + regex;
}

Match::Match(shared_ptr<OnigRegion> region, shared_ptr<regex_t> regex, const string& target) {
    int capture_count = region->num_regs;

    for (int idx = 0; idx < capture_count; idx++) {
        _captures.push_back(Range(region->beg[idx] / sizeof(string::value_type), (region->end[idx] - region->beg[idx]) / sizeof(string::value_type)));
    }
}

unsigned int Match::size() const {
    return _captures.size();
}

const Match Match::make_dummy(int position, int length) {
    Match m;
    m._captures.push_back(Range(position, length));

    return m;
}

bool operator==(const Match& match, Match::MatchResult result) { 
    return result == ((match._captures.size() == 0)? Match::NOT_MATCHED : Match::MATCHED);
}

bool operator==(Match::MatchResult result, const Match& match) {
    return match == result;
}

bool operator!=(const Match& match, Match::MatchResult result) { 
    return !(match == result);
}

bool operator!=(Match::MatchResult result, const Match& match) { 
    return !(match == result);
}

Regex::Regex(const string& regex) {
    init(regex, ONIG_OPTION_CAPTURE_GROUP);
}

Regex::Regex(const string& regex, OnigOptionType option) {
    init(regex, option);
}

/* match text with regex
 *
 * Usage:
 * 1. match(target, start_offset)
 *    search the target from start_offset
 *
 * 2. match(target, start_offset, last_end)
 *    search the target from start_offset, and set last match end to last_end. the third is to support \G
 *    Normally \G matches start position, to get a perl compatible behaviour, this indicate where previous
 *    matching attempt end (the position should be taken care by user of this API)
 *
 * target       -   the target string to search with
 * start_offset -   the start offset to search
 * last_end     -   the last matched end (defaults to -1)
 *
 */
const Match Regex::match(const string& target, int start_offset, int last_end) const {
    if (_regex == nullptr) throw InvalidRegexException("can't match using an empty regex");

    UChar* str = (UChar*) target.c_str();
    UChar* start = (UChar*) (target.c_str() + start_offset);
    UChar* end = (UChar*) (target.c_str() + target.size());
    UChar* gpos = (last_end < 0) ? start : (UChar*) (target.c_str() + last_end);

    shared_ptr<OnigRegion> region(onig_region_new(), [](OnigRegion* ptr) { onig_region_free(ptr, true);});
    if (region == nullptr) throw ShlException("fatal error: can't create OnigRegion");

    int ret_code = onig_search_gpos(_regex.get(), str, end, gpos, start, end, region.get(), ONIG_OPTION_NONE);
    if (ret_code != ONIG_MISMATCH) {
        return Match(region, _regex, target);
    }

    return Match();
}

const string& Regex::source() const {
    return _source;
}

void Regex::init(const string& regex, OnigOptionType option) {
    OnigErrorInfo oni_error;
    UChar* pattern = (UChar*) regex.c_str();
    UChar* pattern_end = (UChar*) (regex.c_str() + regex.size());
    int ret_code;

    
    regex_t* compiled_regex;
    ret_code = onig_new(&compiled_regex, pattern, pattern_end, option, ONIG_ENCODING_UTF8, ONIG_SYNTAX_DEFAULT, &oni_error);
    if (ret_code != ONIG_NORMAL) throw InvalidRegexException("invalid regex: " + onig_error_string(ret_code, &oni_error, regex));

    _regex = shared_ptr<regex_t>(compiled_regex, [](regex_t* ptr) { onig_free(ptr);});
    _source = regex;
}

EndPatternRegex::EndPatternRegex(const string& regex) {
    _has_backref = check_has_backref(regex);
    _original_regex = regex;

    if (!_has_backref) {
        _cached_regex = Regex(regex);
    }
}

const Match EndPatternRegex::match(const Match& last_match, const string& target, int start, int last_end) const {
    if (_has_backref) {
        string expanded = expand_backref(last_match, target);
        Regex dynamic_regex(expanded);
        return dynamic_regex.match(target, start, last_end);
    } else {
        return _cached_regex.match(target, start, last_end);
    }
}

bool EndPatternRegex::check_has_backref(const string& regex) const {
    bool escaped = false;
    for (char ch : regex) {
        if (ch == '\\') {
            escaped = !escaped;
        } else {
            if (escaped && isdigit(ch)) return true;
            escaped = false;
        }
    }
    return false;
}

int get_int(const char* str, int* size) {
    int pos = 0;
    int value = 0;

    for(; str[pos] != 0 && isdigit(str[pos]); pos++) {
        value *= 10;
        value += (str[pos] - '0');
    }

    *size = pos;
    return value;
}

string EndPatternRegex::expand_backref(const Match& match, const string& target) const {
    string expanded_regex;
    bool escaped = false;

    for (size_t pos = 0; pos < _original_regex.size(); pos++) {
        char ch = _original_regex[pos];
        if (ch == '\\') {
            escaped = !escaped;
        } else {
            if (escaped && isdigit(ch)) {
                int size = 0;
                size_t capture_num = get_int(_original_regex.c_str() + pos, &size);
                pos += (size - 1);

                if (capture_num > match.size()) throw InvalidRegexException("trying to back reference a not existed group");
                expanded_regex += match[capture_num].substr(target);
            } else if (escaped) {
                expanded_regex += '\\';
                expanded_regex += ch;
            } else {
                expanded_regex += ch;
            }
            escaped = false;
        }
    }

    return expanded_regex;
}

}
