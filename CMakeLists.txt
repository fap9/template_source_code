cmake_minimum_required(VERSION 3.10)
project(template_generating CXX)
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
#find_package(Threads REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_definitions(FONT_PATH="${CMAKE_CURRENT_SOURCE_DIR}/arial.ttf")  # needed by sf::text


# source code
set(SOURCES
    source/main.cpp
    source/template_utils.cpp
    source/perlin_noise.cpp
    source/template_visualizing.cpp
    source/template_generating.cpp
    source/template_generating_dent.cpp
    source/template_generating_crack.cpp
    source/template_generating_paint.cpp
    source/interpolation.cpp
)

# includes
set(HEADER_FILES 
    includes/template_utils.h
    includes/perlin_noise.h
    includes/template_types.h
    includes/template_visualizing.h
    includes/template_generating.h
    includes/template_generating_dent.h
    includes/template_generating_crack.h
    includes/template_generating_paint.h
    includes/interpolation.h
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADER_FILES})
target_compile_options(${PROJECT_NAME} PRIVATE -W -Wall -Wextra -Werror) # -O3
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_include_directories(${PROJECT_NAME} PRIVATE includes)
target_link_libraries(${PROJECT_NAME} PRIVATE sfml-graphics sfml-window sfml-system)
