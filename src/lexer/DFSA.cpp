// fmt
#include <fmt/core.h>
#include <fmt/format.h>

// vought
#include <common/Assert.hpp>
#include <lexer/DFSA.hpp>

namespace Vought {

DFSA::DFSA(int noOfStates, int noOfCategories,
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

static size_t intStringLen(int integer) {
    size_t length = 1;

    while (0 != (integer /= 10))
        length++;

    return length;
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

int DFSA::getTransition(int state,
                        std::vector<int> const& categories) const {
    ABORTIF(!isValidState(state),
            fmt::format("state {} does not exist", state));

    for (auto const& category : categories) {
        ABORTIF(!isValidState(state),
                fmt::format("category {} does not exist",
                            category));

        std::vector<int> transitions =
            mTransitionTable[state];

        int state = transitions[category];

        if (state != INVALID_STATE) {
            return state;
        }
    }

    return INVALID_STATE;
}

void DFSA::print() const {
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

}  // namespace Vought
