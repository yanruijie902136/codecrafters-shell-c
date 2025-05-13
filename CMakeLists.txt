cmake_minimum_required(VERSION 3.13)

project(codecrafters-shell LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED True)

set(SOURCE_FILES
    "src/autocmp.c"
    "src/cmd.c"
    "src/main.c"
    "src/misc.c"
    "src/parse.c"
    "src/ptr_array.c"
    "src/scan.c"
    "src/token.c"
    "src/trie.c"
    "src/xmalloc.c"
)

add_executable(shell ${SOURCE_FILES})
target_include_directories(shell PRIVATE src)
target_link_libraries(shell PRIVATE readline)
