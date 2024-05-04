#ifndef _GENETIC_ALGORITHM_H_
#define _GENETIC_ALGORITHM_H_

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>

namespace GeneticAlgorithm {

	class Individual {
		void* data;
		float fitness;
	public:
		Individual(void* _data);
		void destroy(void destroy_function(void* data));
		void* getData();
		void setData(void destroy_function(void* _data), void* _data);
		float getFitness();
		void setFitness(float _fitness);
		void addFitness(float _fitness);
	};
	class GeneticAlgorithm {
		int generation;
		int n_population;
		Individual** population;
		void* (*create_individual_function)();
		void (*destroy_individual_function)(void*);
		float (*fitness_function)(void*);
	private:
		void orderByFitnessQuickSort(int began, int end);
		Individual* tournament(int n_candidates);
	public:
		GeneticAlgorithm(int population_size, void* createIndividual(), void destroyIndividual(void*), float fitnessFunction(void*));
		void destroy();
		void eraseFitness();
		int getPopulationSize();
		Individual** getPopulation();
		int getGeneration();
		void* getBest();
		void orderByFitness();
		void runGen();
		void repocreate(int n_reprocreate, void* createSonFunction(void*, void*));
		void run(int generations, int n_reprocreation, void* createSonFunction(void*, void*), int max_fitness=0);
		void promptPopulation(void promptIndividual(void*));
	};
}

#endif // !_GENETIC_ALGORITHM_H_
