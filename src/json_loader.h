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
    static void process(JsonObject& root, const json_value* value);
};

}


// Protect the json_value pointer from been leaked
// by an exception.
class JsonValueGuard {
public:
    JsonValueGuard(json_value* value):_value(value) {};
    ~JsonValueGuard() {
        if (_value != nullptr) json_value_free(_value);
    }
    json_value* get() { return _value; };

private:
    json_value* _value;
};

#endif
