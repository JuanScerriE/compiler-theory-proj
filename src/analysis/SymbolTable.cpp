// vought
#include <analysis/SymbolTable.hpp>
#include <common/Abort.hpp>
#include <optional>

namespace PArL {

SymbolTable* SymbolTable::getEnclosing() const {
    return mEnclosing;
}

void SymbolTable::setEnclosing(SymbolTable* enclosing) {
    mEnclosing = enclosing;
}

SymbolTable::Type SymbolTable::getType() const {
    return mType;
}

void SymbolTable::setType(Type type) {
    mType = type;
}

std::optional<std::string> SymbolTable::getName() const {
    abortIf(
        mType == Type::FUNCTION && !mName.has_value(),
        "function scope must have a name"
    );

    return mName;
}

void SymbolTable::setName(std::string name) {
    mName = name;
}

// void SymbolTable::addInsertRule(
//     std::initializer_list<Rule> rules) {
//     for (auto const& rule : rules) {
//         mInsertRules.push_back(rule);
//     }
// }
//
// void SymbolTable::addInsertRule(
//     std::vector<Rule> const& rules) {
//     for (auto const& rule : rules) {
//         mInsertRules.push_back(rule);
//     }
// }
//
// void SymbolTable::addSearchRule(
//     std::initializer_list<Rule> rules) {
//     for (auto const& rule : rules) {
//         mSearchRules.push_back(rule);
//     }
// }
//
// void SymbolTable::addSearchRule(
//     std::vector<Rule> const& rules) {
//     for (auto const& rule : rules) {
//         mSearchRules.push_back(rule);
//     }
// }

// std::vector<Rule> SymbolTable::getInsertRules() const {
//     return mInsertRules;
// }
// //
// std::vector<Rule> SymbolTable::getSearchRules() const {
//     return mSearchRules;
// }

bool SymbolTable::isGlobalScope() const {
    return mEnclosing == nullptr;
}

void SymbolTable::addIdentifier(
    std::string const& identifier,
    Signature const& signature
) {
    abortIf(
        mMap.count(identifier) > 0,
        "{} is already a registered identifier", identifier
    );

    mMap.insert({identifier, signature});
}

std::optional<Signature> SymbolTable::findIdentifier(
    std::string const& identifier
) const {
    if (mMap.count(identifier) > 0) {
        return mMap.at(identifier);
    }

    return {};
}

// std::optional<Signature> SymbolTable::findIdenfitier(
//     std::string const& identifier) const {
//     SymbolTable const* currentScope = this;
//
//     std::vector<Rule const*> searchRules{};
//
//     while (currentScope != nullptr) {
//         std::optional<Signature> sig =
//             currentScope->_findIdentifier(identifier);
//
//         if (sig.has_value()) {
//             for (auto& rule : searchRules) {
//                 if (!rule->check(identifier,
//                 sig.value())) {
//                     fmt::println(stderr,
//                                  "at identifier {}: {}",
//                                  identifier,
//                                  rule->message);
//                 }
//             }
//
//             return sig;
//         }
//
//         for (auto& rule : currentScope->mSearchRules) {
//             searchRules.push_back(&rule);
//         }
//
//         currentScope = currentScope->mEnclosing;
//     }
//
//     return {};
// }

}  // namespace PArL
