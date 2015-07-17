#ifndef _JSON_LOADER_
#define _JSON_LOADER_

#include <json.h>
#include <string>
#include <functional>

#include "json_object.h"

using std::string;
using std::function;

namespace shl {

class JsonLoader {
public:
    JsonObject load(const string& json);

private:
    static void process_toplevel(JsonObject& root, json_value* value);
    static void for_fields(json_value* value, function<void(const string&, json_value*)> process);
    
};

}

#endif
