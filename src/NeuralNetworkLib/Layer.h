#ifndef _LAYER_H_
#define _LAYER_H_

#include "Array2DLib/Array2D.h"
#include "ActivationFunction.h"



namespace Layer {

	enum LayerType {
		dense = 0
	};

	class Layer {
	public:
		LayerType type = dense;
		void* data;

		Layer(LayerType _type, void* layer);
		void destroy();
		Array::Array2D predict(Array::Array2D input);
	};

	class Dense
	{
	public:
		int n_inputs;
		int n_outputs;
		Array::Array2D weights;
		Array::Array2D bias;
		void (*activation_function)(void*);

		Dense(int _n_inputs, int _n_outputs, void _activation_function(void*));
		void destroy();
		Array::Array2D predict(Array::Array2D input);
	};
}
#endif // !_LAYER_H_


