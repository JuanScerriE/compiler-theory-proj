#pragma once

// vought
#include <common/Error.hpp>
#include <common/Token.hpp>
#include <lexer/DFSA.hpp>

// std
#include <exception>
#include <functional>
#include <list>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace Vought {

enum Category {
    AND,
    BANG,
    COLON,
    COMMA,
    DIGIT,
    DOT,
    EQUAL,
    GREATER_THAN,
    LEFT_BRACE,
    LEFT_PAREN,
    LESS_THAN,
    LETTER,
    MINUS,
    PIPE,
    PLUS,
    RIGHT_BRACE,
    RIGHT_PAREN,
    SEMICOLON,
    SLASH,
    STAR,
    UNDERSCORE,
    WHITESPACE,
};

class LexerException : public std::exception {
   public:
    LexerException(char const* message)
        : mMessage(message) {
    }
    LexerException(std::string message)
        : mMessage(message) {
    }

    [[nodiscard]] char const* what()
        const noexcept override;

   private:
    std::string mMessage;
};

class Lexer {
   public:
    std::optional<Token> nextToken();

    DFSA getDFSA() const;

    bool hasError() const;

    std::list<Error>& getAllErrors();

   private:
    Lexer();

    Token createToken(std::string lexeme,
                      Token::Type type) const;

    Error createError(std::string messgae,
                      std::string lexeme) const;

    bool isAtEnd(size_t offset) const;

    std::optional<char> nextCharacater(size_t cursor) const;

    int categoryOf(char character) const;

    std::pair<int, std::string> simulateDFSA();

    friend class LexerBuilder;

    // source info
    size_t mCursor = 0;

    int mPrevLine = 1;
    int mLine = 1;
    int mPrevColumn = 1;
    int mColumn = 1;

    bool mFoundNewLine = false;

    std::string mSource;

    // dfsa
    DFSA mDFSA;

    // category checkers
    std::unordered_map<int, std::function<bool(char)>>
        mCategoryToChecker;

    // final state to token type association
    std::unordered_map<int, Token::Type>
        mFinalStateToTokenType;

    // error info
    bool mHasError = false;

    std::list<Error>
        mErrorList;  // TODO: make it a generator(oof
                     // coroutines)?
};

class LexerBuilder {
   public:
    LexerBuilder();

    LexerBuilder& addSource(std::string const& source);

    LexerBuilder& addCategory(
        int category, std::function<bool(char)> checker);

    LexerBuilder& setInitialState(int state);

    LexerBuilder& addTransition(int state, int category,
                                int resultantState);

    LexerBuilder& setStateAsFinal(int state,
                                  Token::Type type);

    LexerBuilder& reset();

    Lexer build();

   private:
    std::string mSource{};  // initialise

    std::unordered_set<int> mStates;

    std::unordered_map<int, std::function<bool(char)>>
        mCategories;

    std::unordered_map<std::pair<int, int>, int>
        mTransitions;

    std::optional<int> mInitialState{};  // initialise

    std::unordered_map<int, Token::Type> mFinalStates;
};

}  // namespace Vought
