#pragma once

// fmt
#include <fmt/core.h>

namespace PArL::core {

#ifdef NDEBUG
template <typename... T>
inline void abort(fmt::format_string<T...>, T&&...) {
}

template <typename... T>
inline void
abort_if(bool, fmt::format_string<T...>, T&&...) {
}
#else
template <typename... T>
inline void
abort(fmt::format_string<T...> fmt, T&&... args) {
    fmt::println(stderr, fmt, args...);
    std::abort();
}

template <typename... T>
inline void abort_if(
    bool cond,
    fmt::format_string<T...> fmt,
    T&&... args
) {
    if (cond) {
        abort(fmt, args...);
    }
}
#endif

class Position {
   public:
    Position(int row, int col)
        : mRow(row), mCol(col) {
    }

    [[nodiscard]] int row() const {
        return mRow;
    }

    [[nodiscard]] int col() const {
        return mCol;
    }

   private:
    int mRow;
    int mCol;
};

class Color {
   public:
    Color(uint8_t r, uint8_t g, uint8_t b)
        : mR(r), mG(g), mB(b) {
    }

    [[nodiscard]] uint8_t r() const {
        return mR;
    }

    [[nodiscard]] uint8_t g() const {
        return mG;
    }

    [[nodiscard]] uint8_t b() const {
        return mB;
    }

   private:
    uint8_t mR;
    uint8_t mG;
    uint8_t mB;
};

enum class Operation {
    ADD,
    AND,
    DIV,
    EQ,
    GE,
    GT,
    LE,
    LT,
    MUL,
    NEQ,
    NOT,
    OR,
    SUB,
};

std::string operationToString(core::Operation op) {
    switch (op) {
        case core::Operation::ADD:
            return "+";
        case core::Operation::AND:
            return "and";
        case core::Operation::DIV:
            return "/";
        case core::Operation::EQ:
            return "==";
        case core::Operation::GE:
            return ">=";
        case core::Operation::GT:
            return ">";
        case core::Operation::LE:
            return "<=";
        case core::Operation::LT:
            return "<";
        case core::Operation::MUL:
            return "*";
        case core::Operation::NEQ:
            return "!=";
        case core::Operation::NOT:
            return "not";
        case core::Operation::OR:
            return "or";
        case core::Operation::SUB:
            return "sub";
    };
}

class PrimitivePtr;

enum class Base {
    BOOL,
    COLOR,
    FLOAT,
    INT,
};

struct Array {
    explicit Array(const PrimitivePtr& type)
        : type(type) {
    }
    size_t size;
    PrimitivePtr type;
};

struct Primitive_ {
    template <typename T>
    [[nodiscard]] bool is() const {
        return std::holds_alternative<T>(data);
    }

    template <typename T>
    [[nodiscard]] T as() const {
        return std::get<T>(data);
    }

    explicit Primitive_() = default;

    template <typename T>
    Primitive_(T const& data)
        : data(data) {
    }

    template <typename T>
    Primitive_& operator=(T const& data_) {
        data = data_;

        return *this;
    }

    std::variant<Base, Array> data;
};

class PrimitivePtr {
    PrimitivePtr(const PrimitivePtr& other) {
        ptr = std::make_unique<Primitive_>();

        copy(ptr.get(), other.ptr.get());
    }

    static void copy(Primitive_* ptr, Primitive_* other) {
        if (other->is<Base>()) {
            ptr->data = other->as<Base>();
        }

        if (other->is<Array>()) {
            Array array{};
        }
    }

    Primitive_& operator*() const {
        return *ptr;
    }

    Primitive_* operator->() const {
        return ptr.get();
    }

    std::unique_ptr<Primitive_> ptr;
};

// using Primitive = std::unique_ptr<Primitive_>;
//
// constexpr auto MakePrimitive =
// std::make_unique<Primitive_>;

std::string primitiveToString(Primitive&);

std::string baseToString(Base base) {
    switch (base) {
        case Base::BOOL:
            return "bool";
        case Base::COLOR:
            return "color";
        case Base::FLOAT:
            return "float";
        case Base::INT:
            return "int";
    };
}

std::string arrayToString(Array array) {
    return primitiveToString(array.type) +
           fmt::format("[{}]", array.size);
}

std::string primitiveToString(Primitive primitive) {
    if (primitive->is<Base>()) {
        return baseToString(primitive->as<Base>());
    }

    if (primitive->is<Array>()) {
        return arrayToString(primitive->as<Array>());
    }

    abort("unreachable");
}

enum class Builtin {
    CLEAR,
    DELAY,
    HEIGHT,
    PRINT,
    RANDOM_INT,
    READ,
    WIDTH,
    WRITE,
    WRITE_BOX,
};

}  // namespace PArL::core
