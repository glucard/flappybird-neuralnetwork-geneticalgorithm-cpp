add_library(FlappyBirdLib
    FlappyBird.cpp
    FlappyBird.hpp
)

target_include_directories(FlappyBirdLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(FlappyBirdLib
    Array2DLib
)