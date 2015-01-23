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

function detect_circle(obj){
	if(typeof(obj) != "object") return;

	if (obj.visit == true) {
		console.log("detected");
		return true;
	}

	for(var key in obj){
		obj.visit = true;
		if (detect_circle(obj[key])) {
			console.log(key);
			obj.visit = null;
			return true;
		}
	}
	obj.visit = null;
}

function get_defs(lang) {
	var lang_defs = [];
	var hljs = require("./tmp/build/lib/index.js");
	var lang_names = hljs.listLanguages();

	for (var i = 0; i < lang_names.length; i++){
		var l = hljs.getLanguage(lang_names[i]);
		try{
			var json = JSON.stringify(l);
			lang_defs.push(l);
		}
		catch(e){
			//console.log(l);
			detect_circle(l);
			console.log(l);
			report_error(e);
		}
	}
	return lang_defs;
}

parse_args(process.argv);
var lang_defs = get_defs();
console.log(lang_defs[0]);
console.log(lang_defs.length);
