// lox
#include <errors/RuntimeError.hpp>
#include <evaluator/Environment.hpp>

namespace Lox {

void Environment::define(std::string const &name,
                         Value const &value) {
    mValues[name] = value;
}

Value Environment::get(Token const &name) const {
    if (mValues.count(name.getLexeme()) == 0) {
        throw RuntimeError(name, "Undefined variable '" +
                                     name.getLexeme() +
                                     "'");
    }

    return mValues.at(name.getLexeme());
}

}  // namespace Lox