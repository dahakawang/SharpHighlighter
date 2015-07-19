#include <iostream>
#include <cstdio>

#include "json_object.h"
#include "json_loader.h"
#include "regex.h"

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

int main() {
    FILE* file = fopen("test/c.json", "rb");
    if(file == nullptr) error("error open file");

    char* buffer;
    int size;
    buffer = read_all(file, size);

    string buf(buffer, buffer + size);
    shl::JsonLoader loader;
    shl::JsonObject object = loader.load(buf);

    string target = "TEST 123";
    shl::Regex regex("\\w+ \\d+");
    shl::Match m = regex.match(target, 0);
    if (m != shl::Match::NOT_MATCHED) {
        cout << m[0].position << m[0].length << endl;
        cout << m[0].substr(target) << endl;
    } else {
        cout << "cannot found" << endl;
    }

    fclose(file);

        
}
