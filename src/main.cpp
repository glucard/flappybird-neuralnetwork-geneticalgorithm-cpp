#include <list>
#include "AnnGA.hpp"
#include "FlappyBird.hpp"
#include "FlappyBirdInterface.hpp"

#define FITNESS_LIMIT 25000

void* createIndividual() {
	int input_shape[2] = { 1,2 };
	NeuralNetwork::NeuralNetwork* nn = new NeuralNetwork::NeuralNetwork(input_shape);
	nn->addLayerDense(2, ActivationFunction::ReLU);
	nn->addLayerDense(1, ActivationFunction::sigmoid);

	return nn;
}

float fitnessFunction(void* data) {
	NeuralNetwork::NeuralNetwork* nn = (NeuralNetwork::NeuralNetwork*)data;

	FlappyBird game(1, RESOLUTION_X, RESOLUTION_Y, TUNNEL_VELOCITY, RESOLUTION_Y / 30, RESOLUTION_X / 10,
		RESOLUTION_Y / 2, 0, 0);

	float fitness = 0;
	Array::Array2D a(FLOAT_TYPE, 0, 0, NULL);
	for (int i = 0; i < 2; i++) {
		while (game.update(GRAVITY_ACCELERATION, BIRD_MAX_SPEED)) {
			a.overwrite(game.getIaInput(*game.bird_list.begin()));
			a.overwrite(nn->predict(a));
			if (((float**)a.data)[0][0] > 0.5) {
				game.bird_list.begin()->flap(BIRD_FLAP_ACCELERATION);
				fitness--;
			}
			if (game.isOnTunnel(*game.bird_list.begin())) fitness++;
			if (fitness > FITNESS_LIMIT) break;
		}
		game.restart();
	}
	return fitness;
}

int main() {
	srand(time(NULL));
	AnnGA::AnnGA annga(50, createIndividual, fitnessFunction);
	FlappyBirdInterce::drawInterfaceGA(&annga);
	annga.destroy();
	return 0;
}