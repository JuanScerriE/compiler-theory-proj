// fmt
#include <fmt/core.h>
#include <fmt/format.h>

// parl
#include <common/Abort.hpp>
#include <lexer/Dfsa.hpp>

namespace PArL {

Dfsa::Dfsa(
    size_t noOfStates, size_t noOfCategories,
    std::vector<std::vector<int>> const& transitionTable,
    int initialState,
    std::unordered_set<int> const& finalStates)
    :
      mNoOfStates(noOfStates),
      mNoOfCategories(noOfCategories),
      mTransitionTable(transitionTable),
      mInitialState(initialState),
      mFinalStates(finalStates) {
}


int Dfsa::getInitialState() const {
    return mInitialState;
}

bool Dfsa::isValidState(int state) const {
    return 0 <= state && state < mNoOfStates;
}

bool Dfsa::isValidCategory(int category) const {
    return 0 <= category && category < mNoOfCategories;
}

bool Dfsa::isFinalState(int state) const {
    return mFinalStates.count(state) > 0;
}

int Dfsa::getTransition(
    int state, std::vector<int> const& categories) const {
    ABORTIF(!isValidState(state),
            fmt::format("state {} does not exist", state));

    for (auto const& category : categories) {
        ABORTIF(!isValidCategory(category),
                fmt::format("category {} does not exist",
                            category));
        int nextState = mTransitionTable[state][category];

        if (nextState != INVALID_STATE) {
            return nextState;
        }
    }

    return INVALID_STATE;
}

static inline size_t intStringLen(size_t integer) {
    size_t length = 1;

    while (0 != (integer /= 10))
        length++;

    return length;
}

void Dfsa::print() const {
    // we use +1 to handle the fact that we will most
    // likely have a '-' in front and that's an extra
    // character.
    size_t length = intStringLen(mNoOfStates) + 1;

    fmt::println("Accepting States:\n\t{}",
                 fmt::join(mFinalStates, ","));

    fmt::println("Initial State:\n\t{}", mInitialState);

    fmt::println("Transition Table:");

    for (int i = 0; i < mNoOfStates; i++) {
        for (int j = -1; j < mNoOfCategories; j++) {
            if (j == -1) {
                fmt::print("\t{0: {1}}: ", i, length);
            } else {
                fmt::print("{0: {1}}",
                           mTransitionTable[i][j], length);

                if (j < mNoOfCategories - 1)
                    fmt::print(", ");
            }
        }

        fmt::print("\n");
    }
}

}  // namespace PArL
