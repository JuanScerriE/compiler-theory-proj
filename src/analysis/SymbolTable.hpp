#pragma once

// vought
#include <analysis/Signature.hpp>

// std
#include <functional>
#include <initializer_list>
#include <optional>
#include <string>

namespace PArL {

using RuleCheck =
    std::function<bool(std::string const&, Signature)>;

class RepeatSymbolException : public std::exception {};

struct Rule {
    std::string message;
    RuleCheck check;
};

class RuleViolation : public std::exception {
   public:
    explicit RuleViolation(char const* message)
        : mMessage(message) {
    }
    explicit RuleViolation(std::string const& message)
        : mMessage(message) {
    }

    [[nodiscard]] char const* what()
        const noexcept override;

   private:
    std::string mMessage;
};

class SymbolTable {
   public:
    friend class SymbolStack;

    void addIdentifier(std::string const& identifier,
                       Signature signature);
    std::optional<Signature> findIdenfitier(
        std::string const& identifier) const;

    void setEnclosing(SymbolTable* enclosing);

    void addInsertRule(std::initializer_list<Rule> rules);
    void addInsertRule(std::vector<Rule> rules);
    void addSearchRule(std::initializer_list<Rule> rules);
    void addSearchRule(std::vector<Rule> rules);

    std::vector<Rule> getInsertRules() const;
    std::vector<Rule> getSearchRules() const;

    bool isGlobalScope() const;

   private:
    std::optional<Signature> _findIdentifier(
        std::string const& identifier) const;

    std::unordered_map<std::string, Signature> mMap{};

    std::vector<Rule> mInsertRules{};
    std::vector<Rule> mSearchRules{};

    SymbolTable* mEnclosing{nullptr};
};

}  // namespace PArL
