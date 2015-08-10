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

    Range(const Range& obj) = default;
    Range(int pos, int len):position(pos), length(len) {};
    Range():position(0),length(0) {};
    int end() const {return position + length;};

    string substr(const string& str) { return str.substr(position, length); };
};

class Match {
public:
    friend class Regex;
    enum MatchResult { MATCHED, NOT_MATCHED };

    Match() = default;
    Range operator[](int capture_index) const { return _captures[capture_index]; };
    unsigned int size() const;
    Range named_capture(const string& name) const { return _named_captures.at(name); };
    static Match make_dummy(int position, int length);
    operator bool() { return _captures.size() != 0; };
    operator MatchResult() { return (_captures.size() == 0)? NOT_MATCHED : MATCHED; };

private:
    vector<Range> _captures;
    map<string, Range> _named_captures;

    Match(shared_ptr<OnigRegion> region, shared_ptr<regex_t> regex, const string& target);
};

class Regex {
public:
    Regex() {};
    Regex(const string& regex);
    Regex(const string& regex, OnigOptionType option);
    const string& source() const;
    bool empty() const { return _source.empty(); };
    Match match(const string& target, int start, int last_end = -1) const;

private:
    shared_ptr<regex_t> _regex;
    string _source;

    void init(const string& regex, OnigOptionType option);
};

}

#endif
