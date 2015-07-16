#include <json.h>
#include <string>
#include <functional>

#include "json_object.h"
#include "json_loader.h"
#include "shl_exception.h"

using std::string;
using std::function;

namespace shl {
JsonObject JsonLoader::load(const string& json) {
    JsonObject object;
    json_value* value = json_parse((const json_char*)json.c_str(), json.size());    


    json_value_free(value);
    return object;
}

void JsonLoader::process_toplevel(JsonObject& root, json_value* value) {
    if(value->type != json_object) throw InvalidGrammarException("grammar definition file should have only one object as root");

    for_fields(value, [](const string& key, json_value* value){
        //if (key == "scopeName")
    });

    
}

void JsonLoader::for_fields(json_value* value, function<void(const string&, json_value*)> process ) {
    for(int i = 0; i < value->u.object.length; i++) {
        process(value->u.object.values[i].name, value->u.object.values[i].value);
    }
}


}
