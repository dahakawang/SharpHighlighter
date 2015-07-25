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
    int end() {return position + length;};

    string substr(const string& str) { return str.substr(position, length); };
};

class Match {
public:
    static Match NOT_MATCHED;

    Match():_matched(false) {};
    Match(shared_ptr<OnigRegion> region, shared_ptr<regex_t> regex, const string& target);
    bool operator==(const Match& lh) const;
    bool operator!=(const Match& lh) const { return !(*this == lh); };
    Range operator[](int capture_index) const;
    Range& operator[](int capture_index) { return _captures[capture_index]; };
    Range operator[](const string& name) const;
    unsigned int size() const;
    static Match make_dummy(int position, int length);

private:
    vector<Range> _captures;
    map<string, Range> _named_captures;
    bool _matched;
};

class Regex {
public:
    Regex() {};
    Regex(const string& regex);
    Regex(const string& regex, OnigOptionType option);
    const string& source() const;
    bool empty() const { return _source.empty(); };
    Match match(const string& target, int start) const;

private:
    shared_ptr<regex_t> _regex;
    string _source;

    void init(const string& regex, OnigOptionType option);
};

}

#endif
