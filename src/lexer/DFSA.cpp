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
        throw DFSAException(
            fmt::format("state {} does not exist", state));

    if (!isValidCategory(category))
        throw DFSAException(fmt::format(
            "category {} does not exist", category));

    return mTransitionTable[state][category];
}

void DFSA::print() {
    // we use +1 to handle the fact that we will most
    // likely have a - in front and that's an extra
    // character.
    size_t length = intStringLen(mNoOfStates) + 1;

    fmt::println("Accepting States:\n\t{}",
                 fmt::join(mFinalStates, ","));

    fmt::println("Initial State:\n\t{}", mInitialState);

    fmt::println("Transition Table:");

    for (int i = 0; i < mNoOfStates; i++) {
        for (int j = -1; j < mNoOfCategories; j++) {
            if (j == -1) {
                fmt::print("\t{0:0{1}}: ", i, length);
            } else {
                fmt::print("{0:0{1}}",
                           mTransitionTable[i][j], length);

                if (j < mNoOfCategories - 1)
                    fmt::print(", ");
            }
        }

        fmt::print("\n");
    }
}

}  // namespace Vought
