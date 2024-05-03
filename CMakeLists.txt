cmake_minimum_required(VERSION 3.10)

include(FetchContent)

project(flapybird-nn VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

set(SFML_VERSION "2.5.1")

FetchContent_Declare(
    sfml
    GIT_REPOSITORY "https://github.com/SFML/SFML.git"
    GIT_TAG        "${SFML_VERSION}"
)

FetchContent_GetProperties(sfml)
if(NOT sfml_POPULATED)
    FetchContent_Populate(sfml)
    add_subdirectory(${sfml_SOURCE_DIR} ${sfml_BINARY_DIR})
endif()

add_subdirectory(src)  # Add this line to include the subdirectory

add_executable(flapy_nn src/main.cpp)

target_link_libraries(flapy_nn
    PRIVATE
        sfml-graphics
        sfml-system
        sfml-window
    PUBLIC
        AnnGALib
        Array2DLib
        FlappyBirdANNGA
        FlappyBirdLib
        GeneticAlgorithmLib
        NeuralNetworkLib
)

target_compile_options(flapy_nn PRIVATE -Wall)