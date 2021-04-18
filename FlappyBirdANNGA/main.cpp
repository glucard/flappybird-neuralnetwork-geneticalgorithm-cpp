#include <list>
#include "AnnGALib/AnnGA.h"
#include "FlappyBirdLib/FlappyBird.h"
#include "FlappyBirdInterface.h"

void* createIndividual() {
	int input_shape[2] = { 1,3 };
	NeuralNetwork::NeuralNetwork* nn = new NeuralNetwork::NeuralNetwork(input_shape);
	nn->addLayerDense(8, ActivationFunction::ReLU);
	nn->addLayerDense(8, ActivationFunction::ReLU);
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
		}
		game.restart();
	}
	return fitness;
}

void populationFitnessFunction(GeneticAlgorithm::GeneticAlgorithm* ga) {
	GeneticAlgorithm::Individual** population = ga->getPopulation();
	NeuralNetwork::NeuralNetwork* nn;
	int population_size, i_population;
	population_size = ga->getPopulationSize();
	FlappyBird game(population_size, RESOLUTION_X, RESOLUTION_Y, TUNNEL_VELOCITY, RESOLUTION_Y / 30, RESOLUTION_X / 10,
		RESOLUTION_Y / 2, 0, 0);

	for (int i = 0; i < population_size; i++) {
		ga->getPopulation()[i]->setFitness(0);
	}
	Array::Array2D a(FLOAT_TYPE, 0, 0, NULL);
	std::list <Bird> ::iterator i_bird;
	for (int i = 0; i < 1; i++) {
		while (game.update(GRAVITY_ACCELERATION, BIRD_MAX_SPEED)) {
			i_population = 0;
			for (i_bird = game.bird_list.begin(); i_bird != game.bird_list.end(); ++i_bird) {
				nn = (NeuralNetwork::NeuralNetwork*)population[i_population]->getData();
				a.overwrite(game.getIaInput(*i_bird));
				a.overwrite(nn->predict(a));
				if (((float**)a.data)[0][0] > 0.5) {
					i_bird->flap(BIRD_FLAP_ACCELERATION);
					population[i_population]->addFitness(-1.f);
				}
				if (game.isOnTunnel(*i_bird)) population[i_population]->addFitness(1.f);
			}
		}
		game.restart();
	}
}

int main() {
	srand(time(NULL));
	AnnGA::AnnGA annga(100, createIndividual, fitnessFunction);
	annga.runGen();
	annga.promptPopulation();
	annga.run(100, 50, 25000);
	for (int i = 0; i < 50; i++) {
		annga.run(1, 50);
		// FlappyBirdInterce::drawInterfaceNN(annga.getBest());
		FlappyBirdInterce::drawInterfaceGA(annga.getGA());
	}
	annga.destroy();
	return 0;
}