#pragma once

// std
#include <unordered_set>
#include <vector>

// macro definitions
#define INVALID_STATE -1

namespace Vought {

class DFSA {
   public:
    DFSA(int noOfStates, int noOfCategories,
         std::vector<std::vector<int>> const&
             transitionTable,
         int initialState,
         std::unordered_set<int> const& finalStates);

    int getInitialState() const;

    bool isValidState(int state) const;
    bool isValidCategory(int category) const;

    bool isFinalState(int state) const;

    int getTransition(
        int state,
        std::vector<int> const& categories) const;

    void print() const;

   private:
    const int mNoOfStates;      // Q
    const int mNoOfCategories;  // Sigma
    const std::vector<std::vector<int>>
        mTransitionTable;                        // delta
    const int mInitialState;                     // q_0
    const std::unordered_set<int> mFinalStates;  // F
};

}  // namespace Vought
