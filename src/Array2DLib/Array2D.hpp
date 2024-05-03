#ifndef _ARRAY2D_H_
#define _ARRAY2D_H_

#include <stdio.h>
#include <malloc.h>
#include <cstddef>
#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <cmath>

#define FLOAT_TYPE 0

namespace Array {

	void* allocData(int data_type, int array_height, int array_width, void* data_to_alloc);
	void* createRandomData(int data_type, int array_height, int array_width);

	class Array2D
	{
	private:
		int type;
	public:
		int height;
		int width;
		void* data;

		Array2D(int data_type, int array_height, int array_width, void* array_data = NULL);
		Array2D copy();
		void displayData();
		void destroyData();
		void overwrite(Array2D array2d);
		float maxf();
		float absf();
		Array2D crossover(Array2D other_array2d, int cut_point);
		void applyFunction(void (*function)(void*));
		Array2D hadamardProduct(Array2D other_array2d);
		Array2D sum(Array2D other_array2d);
		Array2D dot(Array2D other_array2d);

	};
}
#endif