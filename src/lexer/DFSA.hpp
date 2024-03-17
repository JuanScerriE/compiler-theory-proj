#pragma once

// std
#include <exception>
#include <string>
#include <unordered_set>
#include <vector>

// macro definitions
#define INVALID_STATE -1

namespace Vought {

class DFSAException : public std::exception {
   public:
    DFSAException(char const* message) : mMessage(message) {
    }
    DFSAException(std::string message) : mMessage(message) {
    }

    [[nodiscard]] char const* what()
        const noexcept override;

   private:
    std::string mMessage;
};

class DFSA {
   public:
    int getInitialState() const;

    bool isValidState(int state) const;
    bool isValidCategory(int category) const;

    bool isFinalState(int state) const;

    int getTransition(int state,
                      std::vector<int> categories);

    void print() const;

    friend class LexerBuilder;

   private:
    int mNoOfStates;      // Q
    int mNoOfCategories;  // Sigma
    std::vector<std::vector<int>>
        mTransitionTable;                  // delta
    int mInitialState;                     // q_0
    std::unordered_set<int> mFinalStates;  // F
};

}  // namespace Vought
