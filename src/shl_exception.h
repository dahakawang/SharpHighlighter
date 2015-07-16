#ifndef _SHL_EXCEPTION_
#define _SHL_EXCEPTION_

#include <stdexcept>

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


}

#endif
