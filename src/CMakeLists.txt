add_library(vought_lib STATIC
        # runner/Runner.cpp
        # evaluator/Interpreter.cpp
        # evaluator/Environment.cpp
        # parser/Parser.cpp
        # scanner/Scanner.cpp
        lexer/Lexer.cpp
        # common/Token.cpp
)

target_compile_options(vought_lib PRIVATE)
target_include_directories(vought_lib PUBLIC .)

#---------------------------------

add_executable(vought
    Main.cpp
)

target_compile_options(vought PRIVATE)
target_link_libraries(vought PRIVATE vought_lib)
