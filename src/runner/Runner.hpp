#pragma once

// std
#include <string>

namespace Vought {

class Runner {
   public:
    int runFile(std::string& path);
    int runPrompt();

   private:
    void run(std::string const& source);

    bool mHadScanningError = false;
    bool mHadParsingError = false;
    bool mHadRuntimeError = false;
};

}  // namespace Vought
