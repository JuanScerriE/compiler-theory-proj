#include <algorithm>
#include <lexer/DFSA.hpp>

namespace Vought {

static size_t intStringLen(int integer) {
    size_t length = 1;

    while (0 != (integer /= 10))
        length++;

    return length;
}

static std::string leftPad(std::string const& str,
                           size_t length,
                           char paddingCharacter) {
    if (str.length() >= length) {
        return str;
    } else {
        return std::string(length - str.length(),
                           paddingCharacter) +
               str;
    }
}

char const* DFSA::DFSAException::what() const noexcept {
    return mMessage.c_str();
}

bool DFSA::isValidState(int state) const {
    return START_STATE <= state && state < mNoOfStates;
}

bool DFSA::isValidLetter(int letter) const {
    return START_STATE <= letter && letter < mNoOfLetters;
}

bool DFSA::isAcceptingState(int state) const {
    return std::find(mAcceptingStates.begin(),
                     mAcceptingStates.end(),
                     state) != std::end(mAcceptingStates);
}

DFSA& DFSA::setTransition(int currentState, int letter,
                          int resultantState) {
    if (!isValidState(currentState))
        throw DFSAException("state does not exist");

    if (!isValidState(resultantState))
        throw DFSAException("state does not exist");

    if (!isValidLetter(letter))
        throw DFSAException("letter does not exist");

    mTransitionTable[currentState][letter] = resultantState;

    return *this;
}

DFSA& DFSA::setTransitionAsFinal(int currentState,
                                 int letter,
                                 int resultantState) {
    mAcceptingStates.insert(resultantState);

    return setTransition(currentState, letter,
                         resultantState);
}

int DFSA::getTransition(int currentState,
                        int letter) const {
    if (!isValidState(currentState))
        throw DFSAException("state does not exist");

    if (!isValidLetter(letter))
        throw DFSAException("letter does not exist");

    return mTransitionTable[currentState][letter];
}

std::ostream& operator<<(std::ostream& out,
                         DFSA const& dfsa) {
    // we use +1 to handle the fact that we will most
    // likely have a - in front and that's an extra
    // character.
    size_t length = intStringLen(dfsa.mNoOfStates) + 1;

    out << "Accepting States: ";

    for (int state : dfsa.mAcceptingStates) {
        out << state << ", ";
    }

    out << "\n";

    for (int i = 0; i < dfsa.mNoOfStates; i++) {
        for (int j = 0; j < dfsa.mNoOfLetters; j++) {
            out << leftPad(
                std::to_string(dfsa.mTransitionTable[i][j]),
                length, ' ');

            if (j < dfsa.mNoOfLetters - 1)
                out << ", ";
        }

        out << "\n";
    }

    return out;
}

}  // namespace Vought
