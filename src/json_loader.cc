#include <json.h>
#include <utf8.h>
#include <string>
#include <functional>
#include <iostream>

#include "json_object.h"
#include "json_loader.h"
#include "shl_exception.h"

using std::string;
using std::function;

namespace shl {

static inline void ensure_utf8(const string& str) {
    if (!utf8::is_valid(str.begin(), str.end())) throw InvalidGrammarException("grammar file contains invalid UTF-8 character");
}

static inline void ensure_object(const json_value* value, const string& msg) {
    if (value->type != json_object) throw InvalidGrammarException(msg);
}

static inline void ensure_string(const json_value* value, const string& msg) {
    if (value->type != json_string) throw InvalidGrammarException(msg);
}

static inline void ensure_array(const json_value* value, const string& msg) {
    if (value->type != json_array) throw InvalidGrammarException(msg);
}

static void for_fields(const json_value* value, function<void(const string&, const json_value*)> process ) {
    for(unsigned int i = 0; i < value->u.object.length; i++) {
        process(value->u.object.values[i].name, value->u.object.values[i].value);
    }
}

static void for_each(const json_value* value, function<void(const json_value*)> process) {
    for(unsigned int i = 0; i < value->u.array.length; i++) {
        process(value->u.array.values[i]);
    }
}


static inline string get_string(const json_value* value) {
    return value->u.string.ptr;
}


static inline map<string, map<string, string> > get_captures(const json_value* value) {
    map<string, map<string, string> > capture_list;

    for_fields(value, [&capture_list](const string& key, const json_value* value) {
        ensure_object(value, "every capture definition should be an object");

        map<string, string>  capture;
        for_fields(value, [&capture](const string& key, const json_value* value) {
            capture[key] = get_string(value);
        });

        capture_list[key] = capture;
    });
    
    return capture_list;
}


static inline JsonValueGuard parse_json(const string& json) {
    json_value* value = json_parse((const json_char*)json.c_str(), json.size());
    if (value == nullptr) throw InvalidGrammarException("grammar is not a valid json object");
    return JsonValueGuard(value);
}

JsonObject JsonLoader::load(const string& json) {
    ensure_utf8(json);

    JsonObject object;
    JsonValueGuard value = parse_json(json);

    process(object, value.get());


    return object;
}

void JsonLoader::process(JsonObject& root, const json_value* value) {
    ensure_object(value, "grammar definition file should have only one object as root");

    for_fields(value, [&root](const string& key, const json_value* value){
        if (key == "include") {
            ensure_string(value, "include should be a string");
            root.include = get_string(value);

        } else if (key == "scopeName") {
            ensure_string(value, "scopeName should be a string");
            root.scope_name = get_string(value);

        } else if (key == "fileTypes") {
            ensure_array(value, "fileTypes should be array of String"); 
            for_each(value, [&root](const json_value* value) {
                root.file_types.push_back(get_string(value));
           });
           
        } else if (key == "name") {
            ensure_string(value, "name should be a string");
            root.name = get_string(value);

        } else if (key == "patterns") {
            ensure_array(value, "patterns should be an array");
            for_each(value, [&root](const json_value* value) {
                ensure_object(value, "patterns can only contains objects");
                JsonObject pattern;
                process(pattern, value);
                root.patterns.push_back(pattern);
            });
            
        } else if (key == "repository") {
            ensure_object(value, "repository should be a key-value map");
            for_fields(value, [&root](const string& key, const json_value* value){
                ensure_object(value, "repository item can only be an object");
                JsonObject repo_item;
                process(repo_item, value);
                root.repository[key] = repo_item;
            });

        } else if (key == "match") {
            ensure_string(value, "match should be a regex");
            root.match = get_string(value);
        } else if (key == "captures") {
            ensure_object(value, "captures should be an object");
            root.captures = get_captures(value);
        } else if (key == "begin") {
            ensure_string(value, "begin pattern should be a regex");
            root.begin = get_string(value);
        } else if (key == "begin_captures") {
            ensure_object(value, "begin_captures should be an object");
            root.begin_captures = get_captures(value);
        } else if (key == "end") {
            ensure_string(value, "end pattern should be a regex");
            root.end = get_string(value);
        } else if (key == "end_captures") {
            ensure_object(value, "end_captures should be an object");
            root.end_captures = get_captures(value);
        } else if (key == "content_name") {
            ensure_string(value, "content_name should be a string");
            root.content_name = get_string(value);
        }
    });
}

}
