// std
#include <cstdio>
#include <cstdlib>

// unix
#include <unistd.h>

// PArL
#include <lexer/Lexer.hpp>
#include <runner/Runner.hpp>

int main(int argc, char *argv[]) {
    bool dfsaDbg = false;
    bool lexerDbg = false;
    bool parserDbg = false;

    int opt;

    while ((opt = getopt(argc, argv, "hdlp")) != -1) {
        switch (opt) {
            case 'd':
                dfsaDbg = true;
                break;
            case 'l':
                lexerDbg = true;
                break;
            case 'p':
                parserDbg = true;
                break;
            case 'h':
                /* fallthrough */
            default:
                /* '?' */

                fprintf(stderr,
                        "Usage: %s [-h] [-d] [-l] [-p] "
                        "[file]\n",
                        argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (argc - optind >=
        2) {  // cater for the optional file
        fprintf(stderr,
                "Error: at most one input file can be "
                "provided");
        exit(EXIT_FAILURE);
    }

    PArL::Runner runner(dfsaDbg, lexerDbg, parserDbg);

    if (argc - optind == 1) {
        std::string path(argv[optind]);

        return runner.runFile(path);
    } else {
        return runner.runPrompt();
    }
}
