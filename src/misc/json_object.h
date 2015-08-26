#ifndef _JSON_OBJECT_
#define _JSON_OBJECT_

#include <string>
#include <map>
#include <vector>

using std::vector;
using std::string;
using std::map;

namespace shl {

class JsonObject {
public:
    string name;
    string scope_name;
    vector<string> file_types;
    vector<JsonObject> patterns;
    map<string, JsonObject> repository;

    string include;
    string begin;
    map<string, map<string, string> > begin_captures;
    string end;
    map<string, map<string, string> > end_captures;
    string applyEndPatternLast;
    string content_name;
    string match;
    map<string, map<string, string> > captures;
};

}

#endif
