// fmt
#include <fmt/core.h>
#include <fmt/format.h>

// vought
#include <lexer/DFSA.hpp>

namespace Vought {

static size_t intStringLen(int integer) {
    size_t length = 1;

    while (0 != (integer /= 10))
        length++;

    return length;
}

char const* DFSAException::what() const noexcept {
    return mMessage.c_str();
}

int DFSA::getInitialState() const {
    return mInitialState;
}

bool DFSA::isValidState(int state) const {
    return 0 <= state && state < mNoOfStates;
}

bool DFSA::isValidCategory(int category) const {
    return 0 <= category && category < mNoOfCategories;
}

bool DFSA::isFinalState(int state) const {
    return mFinalStates.count(state) > 0;
}

int DFSA::getTransition(int state, int category) const {
    if (!isValidState(state))
        throw DFSAException("state does not exist");

    if (!isValidCategory(category))
        throw DFSAException("category does not exist");

    return mTransitionTable[state][category];
}

void DFSA::print() {
    // we use +1 to handle the fact that we will most
    // likely have a - in front and that's an extra
    // character.
    size_t length = intStringLen(mNoOfStates) + 1;

    fmt::println("Accepting States: {}",
                 fmt::join(mFinalStates, ","));

    for (int i = 0; i < mNoOfStates; i++) {
        for (int j = 0; j < mNoOfCategories; j++) {
            fmt::print("{0:0{1}}", mTransitionTable[i][j],
                       length);

            if (j < mNoOfCategories - 1)
                fmt::print(", ");
        }

        fmt::print("\n");
    }
}

}  // namespace Vought
