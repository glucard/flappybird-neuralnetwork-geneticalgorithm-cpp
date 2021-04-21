#include "GeneticAlgorithm.h"

namespace GeneticAlgorithm {

	Individual::Individual(void* _data) {
		data = _data;
		fitness = 0;
	}
	void Individual::destroy(void destroy_function(void* _data)) {
		destroy_function(data);
	}
	void* Individual::getData() {
		return data;
	}
	void Individual::setData(void destroy_function(void* _data), void* _data) {
		destroy(destroy_function);
		data = _data;
	}
	float Individual::getFitness() {
		return fitness;
	}
	void Individual::setFitness(float _fitness) {
		fitness = _fitness;
	}
	void Individual::addFitness(float _fitness) {
		fitness += _fitness;
	}

	GeneticAlgorithm::GeneticAlgorithm(int population_size, void* createIndividual(), void destroyIndividual(void*),
	float fitnessFunction(void*)) {
		generation = 0;
		create_individual_function = createIndividual;
		destroy_individual_function = destroyIndividual;
		void* individual_data;
		n_population = population_size;
		fitness_function = fitnessFunction;
		population = (Individual**)malloc(n_population * sizeof(Individual*));
		for (int i = 0; i < n_population; i++) {
			individual_data = create_individual_function();
			population[i] = new Individual(individual_data);
		}
	}
	void GeneticAlgorithm::destroy() {
		for (int i = 0; i < n_population; i++) {
			population[i]->destroy(destroy_individual_function);
			delete population[i];
		}
		free(population);
	}
	void GeneticAlgorithm::eraseFitness() {
		for (int i = 0; i < n_population; i++) {
			population[i]->setFitness(0);
		}
	}
	void GeneticAlgorithm::orderByFitnessQuickSort(int began, int end) {
		int i, j;
		float pivo;
		Individual* aux = NULL;
		i = began;
		j = end - 1;
		pivo = population[(began + end) / 2]->getFitness();
		while (i <= j) {
			while (population[i]->getFitness() > pivo && i < end) i++;
			while (population[j]->getFitness() < pivo && j > began) j--;
			if (i <= j) {
				aux = population[i];
				population[i] = population[j];
				population[j] = aux;
				i++;
				j--;
			}
		}
		if (i < end) orderByFitnessQuickSort(i, end);
		if (j > began) orderByFitnessQuickSort(began, j + 1);
	}
	void GeneticAlgorithm::orderByFitness() {
		orderByFitnessQuickSort(0, n_population);
	}
	Individual* GeneticAlgorithm::tournament(int n_candidates) {
		int total_points = 0;
		for (int i = 1; i <= n_candidates; i++) total_points += i;
		int point_selected = rand() % total_points + 1;
		int i;
		for (i = n_candidates; point_selected - i > 0; i--) point_selected -= i;
		return population[n_candidates - i];
	}
	void GeneticAlgorithm::runGen() {
		float individual_fitness;
		for (int i = 0; i < n_population; i++) {
			individual_fitness = fitness_function(population[i]->getData());
			population[i]->setFitness(individual_fitness);
		}
	}
	void GeneticAlgorithm::repocreate(int n_reprocreate, void* createSonFunction(void*, void*)) {
		if (n_reprocreate > n_population) {
			throw "Reprocreations cant be bigger than population.";
		}
		Individual* father, * mother;
		void* son_data;
		for (int i = n_population - 1; i >= n_reprocreate; i--) {
			father = tournament(n_reprocreate);
			mother = tournament(n_reprocreate);
			while (mother == father) mother = tournament(n_reprocreate);
			son_data = createSonFunction(father->getData(), mother->getData());
			population[i]->setData(destroy_individual_function, son_data);
			population[i]->setFitness(0);
		}
		generation++;
	}
	void GeneticAlgorithm::run(int generations, int n_reprocreation, void* createSonFunction(void*, void*), int max_fitness) {
		for (int i = 0; i < generations; i++) {
			runGen();
			orderByFitness();

			printf("Generation %d: max_fitness = %f.\n", i, population[0]->getFitness());
			if (max_fitness != 0 && max_fitness < population[0]->getFitness()) break;

			repocreate(n_reprocreation, createSonFunction);
		}
		printf("\n");
	}
	void GeneticAlgorithm::promptPopulation(void promptIndividual(void*)) {
		float fitness = 0;
		for (int i = 0; i < n_population; i++) {
			fitness = population[i]->getFitness();
			printf("Fitness: %f, ", fitness);
			promptIndividual(population[i]->getData());
		}
		printf("\n");
	}
	void* GeneticAlgorithm::getBest() {
		orderByFitness();
		return population[0]->getData();
	}
	int GeneticAlgorithm::getGeneration() {
		return generation;
	}
	int GeneticAlgorithm::getPopulationSize() {
		return n_population;
	}
	Individual** GeneticAlgorithm::getPopulation() {
		return population;
	}
}