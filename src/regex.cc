#include <iostream>
#include <memory>
#include "regex.h"
#include "shl_exception.h"

using std::shared_ptr;

namespace shl {

inline string onig_error_string(int code, OnigErrorInfo* error_info) {
    UChar buffer[1024];
    int length = onig_error_code_to_str(buffer, code, error_info);

    return string(buffer, buffer + length);
}

const Match Match::NOT_MATCHED;

Match::Match(shared_ptr<OnigRegion> region, shared_ptr<regex_t> regex, const string& target):_matched(true) {
    int capture_count = region->num_regs;

    for (int idx = 0; idx < capture_count; idx++) {
        _captures.push_back(Range(region->beg[idx] / sizeof(string::value_type), (region->end[idx] - region->beg[idx]) / sizeof(string::value_type)));
    }
}

bool Match::operator==(const Match& lh) const {
    if (!lh._matched && !this->_matched) return true;

    return false;
}

Range Match::operator[](int capture_index) const {
    return _captures.at(capture_index);
}

Range Match::operator[](const string& name) const {
    return _named_captures.at(name);
}

unsigned int Match::size() const {
    return _captures.size();
}

Match Match::make_dummy(int position, int length) {
    Match m;
    m._matched = true;
    m._captures.push_back(Range(position, length));

    return m;
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
Match Regex::match(const string& target, int start_offset, int last_end) const {
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

    return Match::NOT_MATCHED;
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
    if (ret_code != ONIG_NORMAL) throw InvalidRegexException("invalid regex: " + onig_error_string(ret_code, &oni_error));

    _regex = shared_ptr<regex_t>(compiled_regex, [](regex_t* ptr) { onig_free(ptr);});
    _source = regex;
}

}
