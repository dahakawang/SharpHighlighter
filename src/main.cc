#include <iostream>
#include <cstdio>

#include "json_object.h"
#include "json_loader.h"
#include "regex.h"
#include "grammar.h"
#include "grammar_loader.h"
#include "scope.h"
#include "tokenizer.h"
#include "structured_scope.h"

using namespace std;

void error(const char* msg) {
    cerr << msg << endl;
    exit(-1);
}

int file_size(FILE* file) {
    if (fseek(file, 0, SEEK_END) != 0) error("failed to seek");
    int size = ftell(file);
    if (size == -1) error("failed to tell size");
    if (fseek(file, 0, SEEK_SET) != 0) error("failed to seek");

    return size;
}

char* read_all(FILE* file, int& size) {
    size = file_size(file);
    char* buffer = new char[size];
    if(buffer == nullptr) error("can't allocate memeory");
    
    if(fread(buffer, size, 1, file) != 1) error("cannot read file");

    return buffer;
}

string read_file(const string& path) {
    FILE* file = fopen(path.c_str(), "rb");
    if(file == nullptr) error("error open file");

    char* buffer;
    int size;
    buffer = read_all(file, size);

    fclose(file);
    return string(buffer, buffer + size);
}

using namespace shl;

const Pattern& get_map(const map<string, Pattern> m, const char* key) {
    return m.at(key);
}

const Pattern& get_v(const vector<Pattern> v, int i) {
    return v.at(i);
}
int main() {
    string syntax_data = read_file("test/c.json");
    shl::GrammarLoader loader;
    shl::Grammar g = loader.load(syntax_data);

    string source = read_file("test/linker_md.c");

    shl::Tokenizer tokenizer;
    auto tokens = tokenizer.tokenize(g, source);
/*
    for (auto& pair : tokens) {
        cout << "(" << pair.first.position << "," << pair.first.length << "," << pair.first.end() - 1 << ")" << pair.second.name() << endl;
        cout << source.substr(pair.first.position, pair.first.length) << endl;
    }
*/
    StructuredScope structure;
    string markup = structure.to_markup(source, tokens);
    cout << markup << endl;



}
