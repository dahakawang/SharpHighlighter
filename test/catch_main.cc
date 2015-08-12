#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <sstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <iterator>
#include <stdexcept>

using std::ostringstream;
using std::string;
using std::ifstream;
using std::copy;
using std::istreambuf_iterator;
using std::ostreambuf_iterator;


const string load_string(const string& file_path) {
    string path = "test/" + file_path;
    ostringstream stream;
    ifstream fs(path);

    if (!fs.good()) {
        throw std::runtime_error("error access file " + path);
    }

    copy(istreambuf_iterator<char>(fs), istreambuf_iterator<char>(), ostreambuf_iterator<char>(stream));
    
    return stream.str();
}
