add_library(vought_lib STATIC
        runner/Runner.cpp
        lexer/Lexer.cpp
        lexer/DFSA.cpp
        common/Item.cpp
)

# TODO: further improve the structure of the program
# to only expose what is necessary
target_include_directories(vought_lib PUBLIC .)
target_link_libraries(vought_lib PRIVATE fmt::fmt)


#---------------------------------

add_executable(vought
    Main.cpp
)

target_link_libraries(vought PRIVATE vought_lib)
