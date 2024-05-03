add_library(AnnGALib
    AnnGA.cpp
    AnnGA.hpp
)

target_include_directories(AnnGALib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(AnnGALib
    Array2DLib
    NeuralNetworkLib
    GeneticAlgorithmLib
)