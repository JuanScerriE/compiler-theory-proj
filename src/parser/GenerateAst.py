#!/usr/bin/python3

import sys


def define_type(file, class_name, base_name, field_list):
    fields = field_list.split(", ")

    file.write(
        """class {0} : public {1} {{
public:
    {0}(
""".format(
            class_name, base_name
        )
    )

    for i in range(len(fields)):
        field_arr = fields[i].split(" ")
        type_name = field_arr[0]
        name = field_arr[1]

        file.write("        std::unique_ptr<{0}> {1}".format(type_name, name))

        if i < len(fields) - 1:
            file.write(",\n")
        else:
            file.write(")\n")

    for i in range(len(fields)):
        name = fields[i].split(" ")[1]

        if i == 0:
            file.write("        : ")
        else:
            file.write("        , ")

        file.write("{0}(std::move({0}))\n".format(name))

    file.write(
        """    {{
    }}

    void do_accept(VisitorBase& visitor) const override
    {{
        visitor.visit{0}{1}(this);
    }}

""".format(
            class_name, base_name
        )
    )

    for i in range(len(fields)):
        field_arr = fields[i].split(" ")
        type_name = field_arr[0]
        name = field_arr[1]

        file.write(
            """    std::unique_ptr<{0}> {1};
""".format(
                type_name, name
            )
        )

    file.write(
        """};

"""
    )


def forward_declare_types(file, base_name, types):
    file.write(
        """class {0};

""".format(
            base_name
        )
    )

    for type in types:
        type_arr = type.split(":")
        class_name = type_arr[0].strip()
        file.write(
            """class {0};
""".format(
                class_name
            )
        )


def define_visitor(file, base_name, types):
    file.write(
        """
class VisitorBase {
public:
"""
    )

    for type in types:
        type_name = type.split(":")[0].strip()

        file.write(
            """    virtual void visit{0}{1}({0} const* {2}) = 0;
""".format(
                type_name, base_name, base_name.lower()
            )
        )

    file.write(
        """};

template<typename R>
class Visitor : public VisitorBase {
public:
    R result() const
    {
        return _result;
    }

protected:
    R _result;
};

"""
    )


def define_ast(output_dir, base_name, types):
    path = output_dir + "/" + base_name + ".hpp"

    file = open(path, "w")

    file.write(
        """#pragma once

// std
#include <memory>

// lox
#include "Object.hpp"
#include "Token.hpp"

namespace Lox {{

""".format(
            base_name
        )
    )

    forward_declare_types(file, base_name, types)

    define_visitor(file, base_name, types)

    file.write(
        """class {0} {{
public:
    template<typename R>
    R accept(Visitor<R>& visitor) const
    {{
        do_accept(visitor);
        return visitor.result();
    }}

    virtual void do_accept(VisitorBase& visitor) const = 0;

    virtual ~{0}() {{}};
}};

""".format(
            base_name
        )
    )

    for type in types:
        type_arr = type.split(":")
        class_name = type_arr[0].strip()
        field_list = type_arr[1].strip()
        define_type(file, class_name, base_name, field_list)

    file.write("} // namespace Lox")

    file.close()


def main():
    if len(sys.argv) != 2:
        print("usage: generate_ast <output directory>", file=sys.stderr)
        return 64

    output_dir = sys.argv[1]

    define_ast(
        output_dir,
        "Expr",
        [
            "Binary : Expr left, Token oper, Expr right",
            "Grouping : Expr expr",
            "Literal : Object object",
            "Unary : Token oper, Expr right",
        ],
    )

    return 0


if __name__ == "__main__":
    sys.exit(main())
