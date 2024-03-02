#pragma once

#include <common/Item.hpp>
#include <exception>
#include <iostream>
#include <ostream>
#include <set>
#include <string>
#include <vector>

#define INVALID_STATE -1
#define START_STATE 0

namespace Vought {

class DFSA {
   public:
    class DFSAException : public std::exception {
       public:
        DFSAException(char const* message)
            : mMessage(message) {
        }
        DFSAException(std::string message)
            : mMessage(message) {
        }

        [[nodiscard]] char const* what()
            const noexcept override;

       private:
        std::string mMessage;
    };

    explicit DFSA(int noOfStates, int noOfLetters)
        : mNoOfStates(noOfStates),
          mNoOfLetters(noOfLetters),
          mTransitionTable(std::vector<std::vector<int>>(
              noOfStates,
              std::vector<int>(noOfLetters,
                               INVALID_STATE))) {
    }

    bool isValidState(int state) const;
    bool isValidLetter(int letter) const;
    bool isAcceptingState(int state) const;

    DFSA& setTransition(int currentState, int letter,
                        int resultantState);
    DFSA& setTransitionAsFinal(int currentState, int letter,
                               int resultantState);

    int getTransition(int currentState, int letter) const;

    friend std::ostream& operator<<(std::ostream& out,
                                    DFSA const& dfsa);

   private:
    int mNoOfStates;
    int mNoOfLetters;
    std::set<int> mAcceptingStates;
    std::vector<std::vector<int>> mTransitionTable;
};

}  // namespace Vought
