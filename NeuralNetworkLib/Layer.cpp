#include "Layer.h"

namespace Layer {

	Layer::Layer(LayerType _type, void* layer) {
		type = _type;
		data = layer;
	}
	void Layer::destroy() {
		switch (type)
		{
		case dense:
			((Dense*)data)->destroy();
			delete (Dense*)data;
			break;
		default:
			break;
		}
	}
	Array::Array2D Layer::predict(Array::Array2D input) {
		Array::Array2D result = input.copy();

		switch (type)
		{
		case dense:
			result.overwrite(((Dense*)data)->predict(input));
			break;
		default:
			break;
		}

		return result;
	}

	Dense::Dense(int _n_inputs, int _n_outputs, void _activation_function(void*)):
		weights(FLOAT_TYPE, _n_inputs, _n_outputs, Array::createRandomData(FLOAT_TYPE, _n_inputs, _n_outputs)),
		bias(FLOAT_TYPE, 1, _n_outputs, Array::createRandomData(FLOAT_TYPE, 1, _n_outputs))
	{
		n_inputs = _n_inputs;
		n_outputs = _n_outputs;
		activation_function = _activation_function;
	}
	void Dense::destroy() {
		weights.destroyData();
		bias.destroyData();
	}
	Array::Array2D Dense::predict(Array::Array2D input) {
		Array::Array2D result = input.dot(weights);
		result.overwrite(result.sum(bias));
		result.applyFunction(activation_function);
		return result;
	}
}