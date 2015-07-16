#include <iostream>
#include <cstdio>

#include "json_object.h"
#include "json_loader.h"

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


    fclose(file);

        
}
