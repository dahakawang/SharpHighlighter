#ifndef _JSON_LOADER_
#define _JSON_LOADER_

#include <json.h>
#include <string>
#include <functional>
#include <json_object.h>

using std::string;
using std::function;

namespace shl {

class JsonLoader {
public:
    const JsonObject load(const string& json);

private:
    static void process(JsonObject& root, const json_value* value);
};

}
#endif
