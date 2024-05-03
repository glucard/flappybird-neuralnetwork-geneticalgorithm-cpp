add_library(FlappyBirdANNGA
    FlappyBirdInterface.cpp
    FlappyBirdInterface.hpp
)

target_include_directories(FlappyBirdANNGA PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(FlappyBirdANNGA
    PUBLIC
        FlappyBirdLib
        AnnGALib
    PRIVATE
        sfml-graphics
        sfml-system
        sfml-window
)