#ifndef _REGEX_H_
#define _REGEX_H_

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <oniguruma.h>

using std::string;
using std::map;
using std::vector;
using std::shared_ptr;

namespace shl {


struct Range {
    int position;
    int length;

    Range(int pos, int len):position(pos), length(len) {};
    Range():position(0),length(0) {};
    int end() const {return position + length;};
    bool operator==(const Range& rh) const {return position == rh.position && length == rh.length;};
    bool operator!=(const Range& rh) const { return !(*this == rh);};

    const string substr(const string& str) const { return str.substr(position, length); };
};

class Match {
public:
    enum MatchResult { MATCHED, NOT_MATCHED };
    friend class Regex;
    friend bool operator==(const Match& match, MatchResult result);

    Match() = default;
    const Range operator[](int capture_index) const { return _captures[capture_index]; };
    unsigned int size() const;
    static const Match make_dummy(int position, int length);

private:
    vector<Range> _captures;

    Match(shared_ptr<OnigRegion> region, shared_ptr<regex_t> regex, const string& target);
};

bool operator==(const Match& match, Match::MatchResult result);
bool operator==(Match::MatchResult result, const Match& match);
bool operator!=(const Match& match, Match::MatchResult result);
bool operator!=(Match::MatchResult result, const Match& match);

class Regex {
public:
    Regex() {};
    Regex(const string& regex);
    Regex(const string& regex, OnigOptionType option);
    const string& source() const;
    bool empty() const { return _source.empty(); };
    const Match match(const string& target, int start, int last_end = -1) const;

private:
    shared_ptr<regex_t> _regex;
    string _source;

    void init(const string& regex, OnigOptionType option);
};


class EndPatternRegex {
public:
    EndPatternRegex():_has_backref(false) {};
    EndPatternRegex(const string& regex);
    const string& source() const { return _original_regex; };
    bool empty() const { return _original_regex.empty(); };
    const Match match(const Match& last_match, const string& target, int start, int last_end = -1) const;
    bool has_backref() const { return _has_backref; };

private:
    Regex _cached_regex; // if end pattern has no back reference, use this pre-compiled regex
    string _original_regex;
    bool _has_backref;

    bool check_has_backref(const string& regex) const;
    string expand_backref(const Match& match, const string& target) const;
    
};

}

#endif
