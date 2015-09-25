#ifndef _SHL_EXCEPTION_
#define _SHL_EXCEPTION_

#include <stdexcept>
#include <string>

using std::string;
using std::runtime_error;

namespace shl {

class ShlException : public runtime_error {
public:
    explicit ShlException(const string& msg):runtime_error(msg){};
};


class InvalidGrammarException : public ShlException {
public:
    explicit InvalidGrammarException(const string& msg):ShlException(msg){};
};

class InvalidSourceException : public ShlException {
public:
    explicit InvalidSourceException(const string& msg):ShlException(msg){};
};


class InvalidRegexException : public ShlException {
public:
    explicit InvalidRegexException(const string& msg):ShlException(msg){};
};

class InvalidScopeSelector : public ShlException {
public:
    explicit InvalidScopeSelector(const string& msg):ShlException(msg){};
};


}

#endif
