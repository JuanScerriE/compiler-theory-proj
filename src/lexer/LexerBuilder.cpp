// fmt
#include <fmt/core.h>

// vought
#include <lexer/LexerBuilder.hpp>

namespace Vought {

LexerBuilder& LexerBuilder::addSource(
    std::string const& source) {
    mSource = source;

    return *this;
}

LexerBuilder& LexerBuilder::addCategory(
    int category, std::function<bool(char)> checker) {
    if (category < 0)
        throw LexerException(fmt::format(
            "tried to initialise with negative category {}",
            category));

    mCategories[category] = checker;

    return *this;
}

LexerBuilder& LexerBuilder::setInitialState(int state) {
    if (state < 0)
        throw LexerException(fmt::format(
            "tried to set negative initial state {}",
            state));

    mInitialState = state;

    mStates.insert(state);

    return *this;
}

LexerBuilder& LexerBuilder::addTransition(int state,
                                          int category,
                                          int nextState) {
    if (mCategories.count(category) <= 0)
        throw LexerException(
            fmt::format("tried to add a transition using "
                        "an unregistered category {}",
                        category));
    if (state < 0)
        throw LexerException(
            fmt::format("used negative state {}", state));

    if (nextState < 0)
        throw LexerException(fmt::format(
            "used negative nextState {}", nextState));

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
        if (mCategories.count(category) <= 0)
            throw LexerException(fmt::format(
                "tried to add a transition using "
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
    if (mStates.count(state) <= 0)
        throw LexerException(
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
    if (!mInitialState.has_value())
        throw LexerException(
            "cannot build a lexer with an initial state");

    int noOfStates = mStates.size();
    int noOfCategories = mCategories.size();
    int noOfFinalStates = mFinalStates.size();

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
    DFSA dfsa;

    dfsa.mNoOfStates = noOfStates;
    dfsa.mNoOfCategories = noOfCategories;
    dfsa.mTransitionTable = std::move(transitionTable);
    dfsa.mInitialState = initialStateIndex;
    dfsa.mFinalStates = std::move(finalStateIndices);

    // create lexer
    Lexer lexer;

    lexer.mSource = std::move(mSource);
    lexer.mDFSA = dfsa;
    lexer.mCategoryToChecker =
        std::move(categoryIndexToChecker);
    lexer.mFinalStateToTokenType =
        std::move(finalStateIndexToTokenType);

    reset();

    return lexer;
}

}  // namespace Vought
