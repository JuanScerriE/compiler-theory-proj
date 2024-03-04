// std
#include <iostream>

// lox
#include <lexer/Lexer.hpp>
#include <runner/Runner.hpp>

int main(int argc, char **argv) {
    if (argc > 2) {
        std::cout << "vought: usage vought [script]"
                  << std::endl;

        return 64;
    } else {
        Vought::Runner runner;

        if (argc == 2) {
            std::string path(argv[1]);

            return runner.runFile(path);
        } else {
            return runner.runPrompt();
        }
    }
}
