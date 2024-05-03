#ifndef _NEURAL_NETWORK_H_
#define _NEURAL_NETWORK_H_

#include "Array2D.hpp"
#include "Layer.hpp"

namespace NeuralNetwork {
	class NeuralNetwork
	{
		
	public:
		int input_shape[2];
		int output_shape[2];
		int n_layers;
		Layer::Layer** layers;

		NeuralNetwork(const int *_input_shape);
		void destroy();
		void addLayerDense(int units, void activation_function(void*));
		Array::Array2D predict(Array::Array2D input);
	};

}
#endif // !_NEURAL_NETWORK_H_