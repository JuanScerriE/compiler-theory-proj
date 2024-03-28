add_library(vought_lib STATIC
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
target_link_libraries(vought_lib PRIVATE fmt::fmt)
target_compile_options(vought_lib INTERFACE -Wall -Wextra -Wpedantic -Weffc++ -Wconversion)
target_include_directories(vought_lib PUBLIC .)


#---------------------------------

add_executable(vought
        Main.cpp
)

target_compile_options(vought INTERFACE -Werror -Wall -Wextra -Wpedantic -Weffc++ -Wconversion)
# target_compile_options(vought_lib PRIVATE -Werror)
target_link_libraries(vought PRIVATE vought_lib)
target_link_libraries(vought PRIVATE fmt::fmt)
