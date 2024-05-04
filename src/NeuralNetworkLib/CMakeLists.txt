add_library(NeuralNetworkLib
    ActivationFunction.cpp Layer.cpp NeuralNetwork.cpp
    ActivationFunction.hpp Layer.hpp NeuralNetwork.hpp
)

target_include_directories(NeuralNetworkLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(NeuralNetworkLib
    Array2DLib
)