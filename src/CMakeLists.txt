add_library(parl_lib STATIC
        runner/Runner.cpp
        #        ir_gen/GenVisitor.cpp
        #        ir_gen/Frame.cpp
        #        ir_gen/RefStack.cpp
        #        ir_gen/VarDeclCountVisitor.cpp
        #        ir_gen/InstructionCountVisitor.cpp
        #        preprocess/IsFunctionVisitor.cpp
        #        preprocess/ReorderVisitor.cpp
        analysis/Signature.cpp
        analysis/AnalysisVisitor.cpp
        analysis/SymbolStack.cpp
        analysis/SymbolTable.cpp
        parser/Parser.cpp
        parser/PrinterVisitor.cpp
        lexer/Dfsa.cpp
        lexer/Lexer.cpp
        lexer/LexerBuilder.cpp
        lexer/LexerDirector.cpp
        parl/AST.cpp
        parl/Token.cpp
        parl/Errors.hpp
        parl/Errors.cpp
)

# TODO: further improve the structure of the program
# to only expose what is necessary
target_link_libraries(parl_lib PRIVATE fmt::fmt)
target_compile_options(parl_lib INTERFACE -Wall -Wextra -Wpedantic -Weffc++ -Wconversion)
target_include_directories(parl_lib PUBLIC .)


#---------------------------------

add_executable(parl
        Main.cpp
)

target_compile_options(parl INTERFACE -Werror -Wall -Wextra -Wpedantic -Weffc++ -Wconversion)
target_link_libraries(parl PRIVATE parl_lib)
target_link_libraries(parl PRIVATE fmt::fmt)
