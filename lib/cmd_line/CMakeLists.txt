add_library(
    cmd_line STATIC
    src/cmd_line.hpp
    src/cmd_line.cpp
)
target_compile_features(cmd_line PRIVATE cxx_std_17)
set_property(TARGET cmd_line PROPERTY CXX_STANDARD 20)