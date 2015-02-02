#!/usr/bin/env node

var fs = require("fs");

var output_dir = "";
var input_dir = "";

function report_error(msg) {
	console.error(msg);
	process.exit(-1);
}

function parse_args(args) {
	args = args.slice(2);

	if (args.length == 0) report_error("should run with parameters");
	for (i = 0; i < args.length; i++){
		if (args[i] == "-i") {
			input_dir = args[++i];
		} else if (args[i] == "-o"){
			output_dir = args[++i];
		} else {
			report_error("unknown option");
		}		
	}
}

function list_language(input_dir) {
	var filepath = [];
	var files = fs.readdirSync(input_dir);
	for (var i in files) {
		if (/.*\.js$/.test(files[i])) {
			filepath.push(files[i].substring(0, files[i].length-3));
		}
	}
	return filepath;
}

has_cyclic_ref = false;

function find_ref(obj, path) {
	has_cyclic_ref = true;
	var ref = ["ref"];
	var i = 0;
	var found = false;
	for (i = 0; i < path.length; i++) {
		ref.push(path[i].key); //push path element name
		if (path[i].obj == obj) {
			found = true;
			break;
		}
	}

	if (!found) {
		report_error("there declared to be a cyclic reference in the path " + path + " but we cannot found it");
	}
	return ref.join(":");
}

function shadow_copy(obj) {
	var result = {};
	if (obj.constructor == Array) result = [];

	for (var key in obj) {
		result[key] = obj[key];
	}
	return result;
}

function remove_cyclic_ref(obj, path){
	if(! (obj instanceof Object) || (obj instanceof RegExp)) return obj;
	var new_obj = shadow_copy(obj); // we do not modify this object directly, because this may be a **shared object** that are referenced in other place
	obj.visited = true;

	for(var key in new_obj){
		if (obj[key] == null) continue;
		if (obj[key].visited){
			new_obj[key] = find_ref(obj[key], path);
		} else {
			var child = remove_cyclic_ref(obj[key], path.concat({"key":key, "obj":obj[key]}));
			new_obj[key] = child;
		}
	}

	delete obj.visited;
	return new_obj;
}

var hljs = require("./tmp/build/lib/index.js");
function replacer(key, value) {
	if (value instanceof RegExp) return value.source;
	else return value;
}

function get_defs() {
	var lang_defs = [];
	var lang_names = get_names();

	for (var i = 0; i < lang_names.length; i++){
		var lang_hash = hljs.getLanguage(lang_names[i]);
		try{
			has_cyclic_ref = false;
			lang_hash = remove_cyclic_ref(lang_hash, []);
			lang_hash["name"] = lang_names[i];
			if (has_cyclic_ref) console.error("WARNING: language " + lang_names[i] + " contains cyclic reference");

			var json = JSON.stringify(lang_hash, replacer, "  ");
			lang_defs.push(json);
		}
		catch(e){
			console.error(e.stack);
			report_error(e);
			console.log("Language: " + lang_names[i]);
		}
	}
	return lang_defs;
}

function get_names() {
	return ["objectivec", "cpp", "makefile", "xml", "javascript", "css", "cs", "java"];
}

function write_syntax(names, defs) {
	for (var i = 0; i < names.length; i++) {
		var file = output_dir + "/" + names[i] + ".json";
		fs.writeFileSync(file, defs[i]);
	}
}

parse_args(process.argv);
var lang_defs = get_defs();
var lang_names = get_names();
write_syntax(lang_names, lang_defs);
