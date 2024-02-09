set(GCC_COMPILE_FLAGS "${GCC_COMPILE_FLAGS} /std:c++20")
add_library(
    cmd_line STATIC
    src/cmd_line.hpp
    src/cmd_line.cpp
)
