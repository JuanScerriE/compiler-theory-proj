// std
#include <iostream>

// lox
// #include <runner/Runner.hpp>
#include <lexer/Lexer.hpp>

int main(int argc, char **argv) {
    if (argc > 2) {
        std::cout << "vought: usage vought [script]"
                  << std::endl;

        return 64;
    } else {
        Vought::TransitionTable<3> transition_table;

        transition_table.print();
    }
}
