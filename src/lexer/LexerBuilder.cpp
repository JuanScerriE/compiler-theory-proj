// fmt
#include <fmt/core.h>

// parl
#include <common/Abort.hpp>
#include <lexer/LexerBuilder.hpp>

namespace PArL {

LexerBuilder& LexerBuilder::addCategory(
    int category, std::function<bool(char)> checker) {
    ABORTIF(
        category < 0,
        fmt::format(
            "tried to initialise with negative category {}",
            category));

    mCategories[category] = std::move(checker);

    return *this;
}

LexerBuilder& LexerBuilder::setInitialState(int state) {
    ABORTIF(state < 0,
            fmt::format(
                "tried to set negative initial state {}",
                state));

    mInitialState = state;

    mStates.insert(state);

    return *this;
}

LexerBuilder& LexerBuilder::addTransition(int state,
                                          int category,
                                          int nextState) {
    ABORTIF(mCategories.count(category) <= 0,
            fmt::format("tried to add a transition using "
                        "an unregistered category {}",
                        category));
    ABORTIF(state < 0,
            fmt::format("used negative state {}", state));
    ABORTIF(nextState < 0,
            fmt::format("used negative nextState {}",
                        nextState));

    mStates.insert({state, nextState});

    mTransitions[{state, category}] = nextState;

    return *this;
}

LexerBuilder& LexerBuilder::addTransition(
    int state, std::initializer_list<int> categories,
    int nextState) {
    for (int category : categories)
        addTransition(state, category, nextState);

    return *this;
}

LexerBuilder& LexerBuilder::addComplementaryTransition(
    int state, std::initializer_list<int> categories,
    int nextState) {
    for (int category : categories) {
        ABORTIF(
            mCategories.count(category) <= 0,
            fmt::format("tried to add a transition using "
                        "an unregistered category {}",
                        category));
    }

    for (auto& [category, _] : mCategories) {
        if (std::find(categories.begin(), categories.end(),
                      category) == categories.end())
            addTransition(state, category, nextState);
    }

    return *this;
}

LexerBuilder& LexerBuilder::addComplementaryTransition(
    int state, int category, int nextState) {
    return addComplementaryTransition(state, {category},
                                      nextState);
}

LexerBuilder& LexerBuilder::setStateAsFinal(
    int state, Token::Type type) {
    ABORTIF(mStates.count(state) <= 0,
            fmt::format("tried to add a final state using "
                        "an unregistered state {}",
                        state));

    mFinalStates[state] = type;

    return *this;
}

LexerBuilder& LexerBuilder::reset() {
    mStates.clear();
    mCategories.clear();
    mTransitions.clear();
    mInitialState.reset();
    mFinalStates.clear();

    return *this;
}

Lexer LexerBuilder::build() {
    ABORTIF(!mInitialState.has_value(),
            "cannot build a lexer with an initial state");

    size_t noOfStates = mStates.size();
    size_t noOfCategories = mCategories.size();

    std::vector<std::vector<int>> transitionTable =
        std::vector<std::vector<int>>(
            noOfStates, std::vector<int>(noOfCategories,
                                         INVALID_STATE));

    // state fields
    int initialStateIndex;

    std::unordered_set<int> finalStateIndices;

    std::unordered_map<int, Token::Type>
        finalStateIndexToTokenType;

    std::unordered_map<int, int> stateToIndex;

    int stateIndex = 0;

    for (auto const& state : mStates) {
        stateToIndex[state] = stateIndex;

        if (state == mInitialState)
            initialStateIndex = stateIndex;

        if (mFinalStates.count(state) > 0) {
            finalStateIndices.insert(stateIndex);

            finalStateIndexToTokenType[stateIndex] =
                mFinalStates[state];
        }

        stateIndex++;
    }

    // category fields
    std::unordered_map<int, std::function<bool(char)>>
        categoryIndexToChecker;

    std::unordered_map<int, int> categoryToIndex;

    int categoryIndex = 0;

    for (auto const& [category, type] : mCategories) {
        categoryToIndex[category] = categoryIndex;

        categoryIndexToChecker[categoryIndex] = type;

        categoryIndex++;
    }

    // transition table
    for (auto const& [input, nextState] : mTransitions) {
        int state = input.x;
        int category = input.y;

        transitionTable[stateToIndex[state]]
                       [categoryToIndex[category]] =
                           stateToIndex[nextState];
    }

    // create dfsa
    Dfsa dfsa(noOfStates, noOfCategories, transitionTable,
              initialStateIndex, finalStateIndices);

    // create lexer
    Lexer lexer(std::move(dfsa),
                std::move(categoryIndexToChecker),
                std::move(finalStateIndexToTokenType));

    reset();

    return lexer;
}

}  // namespace PArL
