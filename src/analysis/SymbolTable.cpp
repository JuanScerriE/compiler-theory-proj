// vought
#include <analysis/SymbolTable.hpp>
#include <common/Abort.hpp>
#include <initializer_list>

namespace PArL {

char const* RuleViolation::what() const noexcept {
    return mMessage.c_str();
}

void SymbolTable::setEnclosing(SymbolTable* enclosing) {
    mEnclosing = enclosing;
}

void SymbolTable::addInsertRule(
    std::initializer_list<Rule> rules) {
    for (auto const& rule : rules) {
        mInsertRules.push_back(rule);
    }
}

void SymbolTable::addInsertRule(std::vector<Rule> rules) {
    for (auto const& rule : rules) {
        mInsertRules.push_back(rule);
    }
}

void SymbolTable::addSearchRule(
    std::initializer_list<Rule> rules) {
    for (auto const& rule : rules) {
        mSearchRules.push_back(rule);
    }
}

void SymbolTable::addSearchRule(std::vector<Rule> rules) {
    for (auto const& rule : rules) {
        mSearchRules.push_back(rule);
    }
}

std::vector<Rule> SymbolTable::getInsertRules() const {
    return mInsertRules;
}

std::vector<Rule> SymbolTable::getSearchRules() const {
    return mSearchRules;
}

bool SymbolTable::isGlobalScope() const {
    return mEnclosing == nullptr;
}

void SymbolTable::addIdentifier(
    std::string const& identifier, Signature signature) {
    if (mMap.count(identifier) > 0) {
        throw RepeatSymbolException();
    }

    for (auto& rule : mInsertRules) {
        if (!rule.check(identifier, signature)) {
            fmt::println(stderr, "at identifier {}: {}",
                         identifier, rule.message);
        }
    }

    mMap.insert({identifier, signature});
}

std::optional<Signature> SymbolTable::findIdenfitier(
    std::string const& identifier) const {
    SymbolTable const* currentScope = this;

    std::vector<Rule const*> searchRules{};

    while (currentScope != nullptr) {
        std::optional<Signature> sig =
            currentScope->_findIdentifier(identifier);

        if (sig.has_value()) {
            for (auto& rule : searchRules) {
                if (!rule->check(identifier, sig.value())) {
                    fmt::println(stderr,
                                 "at identifier {}: {}",
                                 identifier, rule->message);
                }
            }

            return sig;
        }

        for (auto& rule : currentScope->mSearchRules) {
            searchRules.push_back(&rule);
        }

        currentScope = currentScope->mEnclosing;
    }

    return {};
}

std::optional<Signature> SymbolTable::_findIdentifier(
    std::string const& identifier) const {
    if (mMap.count(identifier) > 0) {
        return mMap.at(identifier);
    }

    return {};
}

}  // namespace PArL
