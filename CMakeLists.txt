# CMakeList.txt: файл проекта CMake верхнего уровня; проведите глобальную конфигурацию
# и включите сюда подпроекты.
#	Copyright (c) 2010 Anders Bennehag
#	Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php
#
#
cmake_minimum_required (VERSION 3.12)

# Enable Hot Reload for MSVC compilers if supported.

project ("AntOptimizationProject")

# Добавьте источник в исполняемый файл этого проекта.

set(SOURCES main.cpp graph.cpp misc.cpp ants.cpp)
set(HEADERS misc.h graph.h ants.h MersenneTwister.h)

add_executable (AntOptimization ${SOURCES} ${HEADERS})


target_link_libraries(AntOptimization PRIVATE pugs)
target_link_libraries(AntOptimization PRIVATE cmd_line)
set_property(TARGET AntOptimization PROPERTY CXX_STANDARD 20)
add_subdirectory(lib/pugiXML)
add_subdirectory(lib/cmd_line)

