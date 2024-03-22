add_library(vought_lib STATIC
        runner/Runner.cpp
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
        common/AST.cpp
        common/Error.cpp
        common/Item.cpp
        common/Token.cpp
        common/Value.cpp
)

# TODO: further improve the structure of the program
# to only expose what is necessary
target_compile_options(vought_lib INTERFACE -Werror -Wall -Wextra -Wpedantic -Weffc++ -Wconversion)
# target_compile_options(vought_lib PRIVATE -Werror)
target_include_directories(vought_lib PUBLIC .)
target_link_libraries(vought_lib PRIVATE fmt::fmt)


#---------------------------------

add_executable(vought
    Main.cpp
)

target_compile_options(vought INTERFACE -Werror -Wall -Wextra -Wpedantic -Weffc++ -Wconversion)
# target_compile_options(vought_lib PRIVATE -Werror)
target_link_libraries(vought PRIVATE vought_lib)
