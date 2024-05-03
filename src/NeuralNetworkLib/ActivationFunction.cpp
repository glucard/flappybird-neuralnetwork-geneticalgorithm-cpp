#include "ActivationFunction.hpp"

namespace ActivationFunction {
	void ReLU(void* x) {
		float* fx = (float*)x;
		*fx = (*fx >= 0 ? *fx : 0); 
	}

	void sigmoid(void* x) {
		float* fx = (float*)x;
		*fx = 1 / (1 + exp(-*fx));
	}
}