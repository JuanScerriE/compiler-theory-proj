#pragma once

#include <common/Item.hpp>
#include <exception>
#include <lexer/DFSA.hpp>
#include <list>
#include <optional>
#include <string>
#include <unordered_map>
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

    OTHER,
    CATEGORY_SIZE
};

class Lexer {
   public:
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

    explicit Lexer(std::string const& source,
                   DFSA const& DFSA)
        : mSource(source), mDFSA(DFSA) {
    }

    Lexer& createTransition(int state, int letter,
                            int resultantState);
    Lexer& createTransitionAsFinal(int state, int letter,
                                   int resultantState,
                                   Token::Type type);

    std::optional<Token> nextToken();
    std::list<Error>& getAllErrors();

    DFSA getDFSA() const;
    bool hasError() const;

   private:
    Token createToken(Token::Type type) const;
    Token createToken(Token::Type type,
                      std::string lexeme) const;
    Error createError(std::string lexeme,
                      std::string message) const;

    bool isAtEnd() const;
    bool isAtEnd(size_t cursor) const;

    std::optional<char> nextCharacater(size_t cursor) const;
    Category categoryOf(char character) const;

    std::pair<int, std::string> simulateDFSA();
    std::optional<Token> getTokenByFinalState(
        int state, std::string lexeme);

    // source info
    size_t mCurrent = 0;
    int mPrevLine = 1;
    int mLine = 1;
    int mPrevColumn = 1;
    int mColumn = 1;
    bool mFoundNewLine = false;
    std::string mSource;

    // dfsa
    DFSA mDFSA;
    std::unordered_map<int, Token::Type> mStateAssociation;

    // error info
    bool mHasError = false;
    std::list<Error>
        mErrorList;  // TODO: make it a generator(oof
                     // coroutines)?
};

}  // namespace Vought
