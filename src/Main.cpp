// std
#include <iostream>

// lox
// #include <runner/Runner.hpp>

int main(int argc, char **argv) {
    if (argc > 2) {
        std::cout << "lox: usage lox [script]" << std::endl;

        return 64;
    } else {
        // Lox::Runner runner;

        if (argc == 2) {
            std::string path(argv[1]);

            // return runner.runFile(path);
        } else {
            // return runner.runPrompt();
        }
    }
}
