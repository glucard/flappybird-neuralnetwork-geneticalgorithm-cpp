#include "NeuralNetwork.hpp"

namespace NeuralNetwork {
	NeuralNetwork::NeuralNetwork(const int* _input_shape) {
		input_shape[0] = _input_shape[0];
		input_shape[1] = _input_shape[1];
		output_shape[0] = _input_shape[0];
		output_shape[1] = _input_shape[1];
		n_layers = 0;
		layers = (Layer::Layer**)malloc(sizeof(Layer::Layer*));
	}
	void NeuralNetwork::destroy() {
		for (int i = 0; i < n_layers; i++) {
			layers[i]->destroy();
			delete layers[i];
		}
		free(layers);
	}
	void NeuralNetwork::addLayerDense(int units, void activation_function(void*)) {
		if (output_shape[0] != 1){
			printf("rola n");
			throw "rola n";
		}
		Layer::Dense* layer = new Layer::Dense(output_shape[1], units, activation_function);
		n_layers += 1;
		layers = (Layer::Layer**)realloc(layers, n_layers * sizeof(Layer::Layer*));
		layers[n_layers - 1] = new Layer::Layer(Layer::dense, layer);

		output_shape[1] = units;
	}
	Array::Array2D NeuralNetwork::predict(Array::Array2D input) {
		Array::Array2D result = input.copy();
		for (int i = 0; i < n_layers; i++) {
			result.overwrite(layers[i]->predict(result));
		}
		return result;
	}
}