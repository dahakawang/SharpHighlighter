#ifndef _REGEX_H_
#define _REGEX_H_

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <oniguruma.h>

using std::wstring;
using std::map;
using std::vector;
using std::shared_ptr;

namespace shl {


struct Range {
    int position;
    int length;

    Range(int pos, int len):position(pos), length(len) {};
    Range():position(0),length(0) {};
};

class Match {
public:
    static Match NOT_MATCHED;

    Match(shared_ptr<OnigRegion> region, shared_ptr<regex_t> regex, const wstring& target);
    bool operator==(const Match& lh) const;
    Range operator[](int capture_index) const;
    Range operator[](const wstring& name) const;

private:
    vector<Range> _captures;
    map<wstring, Range> _named_captures;
    bool _matched;

    Match():_matched(false) {};
};

class Regex {
public:
    Regex(const wstring& regex);
    Regex(const wstring& regex, OnigOptionType option);
    const wstring& source() const;
    Match match(const wstring& target, int start) const;

private:
    shared_ptr<regex_t> _regex;
    wstring _source;

    void init(const wstring& regex, OnigOptionType option);
};

}

#endif
