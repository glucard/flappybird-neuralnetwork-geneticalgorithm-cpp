#ifndef _ANN_GA_H_
#define _ANN_GA_H_

#include "Array2DLib/Array2D.h"
#include "NeuralNetworkLib/NeuralNetwork.h"
#include "GeneticAlgorithmLib/GeneticAlgorithm.h"

namespace AnnGA {

	void destroyIndividual(void* data);
	void* repocreateIndividual(void* father_data, void* mother_data);
	void promptIndividual(void* data);

	class AnnGA {
		GeneticAlgorithm::GeneticAlgorithm* ga;
	public:
		AnnGA(int population_size, void* createIndividual(), float fitnessFunction(void*));
		void destroy();
		void run(int generations, int repocreate, int max_fitness=0);
		void runGen();
		void promptPopulation();
		NeuralNetwork::NeuralNetwork* getBest();
		GeneticAlgorithm::GeneticAlgorithm* getGA();

		// not safe functions.
		void runPopulation(void populationFitnessFunction(GeneticAlgorithm::GeneticAlgorithm* ga));
	};
}

#endif // !_ANN_GA_H_