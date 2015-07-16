#ifndef _JSON_OBJECT_
#define _JSON_OBJECT_

#include <string>
#include <map>
#include <vector>

using std::vector;
using std::wstring;
using std::map;

namespace shl {

class JsonObject {
public:
    wstring name;
    wstring scope_name;
    vector<wstring> file_types;
    vector<JsonObject> patterns;
    map<wstring, JsonObject> repository;

    wstring include;
    wstring begin;
    map<wstring, map<wstring, wstring>> begin_captures;
    wstring end;
    map<wstring, map<wstring, wstring>> end_captures;
    wstring content_name;
    wstring match;
    map<wstring, map<wstring, wstring>> captures;
};

}

#endif
