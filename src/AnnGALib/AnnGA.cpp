#include "AnnGA.hpp"

namespace AnnGA {

	void destroyIndividual(void* data) {
		NeuralNetwork::NeuralNetwork* nn = (NeuralNetwork::NeuralNetwork*)data;
		nn->destroy();
	}

	void mutateIndividual(void* data) {
		NeuralNetwork::NeuralNetwork* nn = (NeuralNetwork::NeuralNetwork*)data;

		int max_cut_point, cut_point_i, cut_point_j;
		int mutations;
		int layers = nn->n_layers;
		for (int i = 0; i < layers; i++) {
			switch (nn->layers[i]->type)
			{
			case Layer::dense:(
				max_cut_point = ((Layer::Dense*)nn->layers[i]->data)->weights.height * ((Layer::Dense*)nn->layers[i]->data)->weights.width);
				mutations = (int)rand() % max_cut_point;
				for (int j = 0; j < mutations; j++) {
					cut_point_i = (int)rand() % ((Layer::Dense*)nn->layers[i]->data)->weights.height;
					cut_point_j = (int)rand() % ((Layer::Dense*)nn->layers[i]->data)->weights.width;
					((float**)((Layer::Dense*)nn->layers[i]->data)->weights.data)[cut_point_i][cut_point_j] += 1 * ((int)rand() % 10 + 1) / 100.f * (rand() % 2 == 1 ? -1 : 1);
				}
				max_cut_point = ((Layer::Dense*)nn->layers[i]->data)->bias.width;
				mutations = (int)rand() % max_cut_point;
				for (int j = 0; j < mutations; j++) {
					cut_point_j = (int)rand() % ((Layer::Dense*)nn->layers[i]->data)->weights.width;
					((float**)((Layer::Dense*)nn->layers[i]->data)->bias.data)[0][cut_point_j] += 1 * ((int)rand() % 10 + 1) / 100.f * (rand() % 2 == 1 ? -1 : 1);
				}
				break;
			default:
				break;
			}
		}
	}
	void* repocreateIndividual(void* father_data, void* mother_data) {
		NeuralNetwork::NeuralNetwork* father = (NeuralNetwork::NeuralNetwork*)father_data;
		NeuralNetwork::NeuralNetwork* mother = (NeuralNetwork::NeuralNetwork*)mother_data;
		NeuralNetwork::NeuralNetwork* son = new NeuralNetwork::NeuralNetwork(father->input_shape);

		int cut_point;

		int layers = father->n_layers;
		for (int i = 0; i < layers; i++) {
			switch (father->layers[i]->type)
			{
			case Layer::dense:
				son->addLayerDense(((Layer::Dense*)father->layers[i]->data)->n_outputs, ((Layer::Dense*)father->layers[i]->data)->activation_function);

				cut_point = (int)rand() % (((Layer::Dense*)father->layers[i]->data)->weights.height * ((Layer::Dense*)father->layers[i]->data)->weights.width);
				((Layer::Dense*)son->layers[i]->data)->weights.overwrite(((Layer::Dense*)father->layers[i]->data)->weights.crossover(((Layer::Dense*)mother->layers[i]->data)->weights, cut_point));

				cut_point = rand() % (((Layer::Dense*)father->layers[i]->data)->bias.height * ((Layer::Dense*)father->layers[i]->data)->bias.width);
				((Layer::Dense*)son->layers[i]->data)->bias.overwrite(((Layer::Dense*)father->layers[i]->data)->bias.crossover(((Layer::Dense*)mother->layers[i]->data)->bias, cut_point));

			default:
				break;
			}
		}
		mutateIndividual(son);
		return son;
	}

	void promptIndividual(void* data) {
		NeuralNetwork::NeuralNetwork* nn = (NeuralNetwork::NeuralNetwork*)data;
		printf("\n");
	}

	AnnGA::AnnGA(int population_size, void* createIndividual(), float fitnessFunction(void*)) {
		ga = new GeneticAlgorithm::GeneticAlgorithm(population_size, createIndividual, destroyIndividual, fitnessFunction);
	}
	void AnnGA::destroy() {
		ga->destroy();
		delete ga;
	}
	void AnnGA::run(int generations, int repocreate, int max_fitness) {
		ga->run(generations, repocreate, repocreateIndividual, max_fitness);
	}
	void AnnGA::runGen() {
		ga->runGen();
		ga->orderByFitness();
	}
	void AnnGA::promptPopulation() {
		// ga->orderByFitness();
		ga->promptPopulation(promptIndividual);
	}
	NeuralNetwork::NeuralNetwork* AnnGA::getBest() {
		return (NeuralNetwork::NeuralNetwork*)ga->getBest();
	}
	GeneticAlgorithm::GeneticAlgorithm* AnnGA::getGA() {
		return ga;
	}
	void AnnGA::runPopulation(void populationFitnessFunction(GeneticAlgorithm::GeneticAlgorithm* ga)) {
		populationFitnessFunction(ga);
		ga->orderByFitness();
	}
}