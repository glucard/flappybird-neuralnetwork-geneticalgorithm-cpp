#include "Array2D.hpp"

namespace Array {
	void* allocData(int data_type, const int array_height, const int array_width, void* data_to_alloc) {
		void* data;
		switch (data_type) {
		case FLOAT_TYPE:
			data = malloc(array_height * sizeof(float*));
			for (int i = 0; i < array_height; i++) {
				((float**)data)[i] = (float*)malloc(array_width * sizeof(float));
				for (int j = 0; j < array_width; j++) {
					((float**)data)[i][j] = ((float*)data_to_alloc)[i * array_width + j];
				}
			}
			break;
		default:
			throw "This array type doesnt exist";
			break;
		}
		return data;
	}

	void* createRandomData(int data_type, int array_height, int array_width) {
		void* data;
		switch (data_type) {
		case FLOAT_TYPE:
			data = malloc(array_height * sizeof(float*));
			for (int i = 0; i < array_height; i++) {
				((float**)data)[i] = (float*)malloc(array_width * sizeof(float));
				for (int j = 0; j < array_width; j++) {
					((float**)data)[i][j] = (float)rand() / RAND_MAX * (rand()%2 == 0 ? 1 : -1);
				}
			}
			break;
		default:
			throw "This array type doesnt exist";
			break;
		}
		return data;
	}


	Array2D::Array2D(int data_type, int array_height, int array_width, void* array_data) {
		type = data_type;
		height = array_height;
		width = array_width;
		data = array_data;
	}

	void Array2D::displayData() {
		switch (type) {
		case FLOAT_TYPE:
			float** float_data = (float**)data;
			printf("{");
			for (int i = 0; i < height; i++) {
				if (i != 0) {
					printf(" ");
				}
				printf("{");
				for (int j = 0; j < width; j++) {
					printf(" %f ", float_data[i][j]);
				}
				printf("}");
				if (i + 1 < height) {
					printf("\n");
				}
			}
			printf("}\n");
			break;
		}
	}

	void Array2D::destroyData() {
		switch (type) {
		case FLOAT_TYPE:
			for (int i = 0; i < height; i++) {
				free(((float**)data)[i]);
			}
			free((float**)data);
			break;
		default:
			break;
		}
	}

	void Array2D::overwrite(Array2D array2d) {
		destroyData();
		type = array2d.type;
		height = array2d.height;
		width = array2d.width;
		data = array2d.data;
	}

	float Array2D::maxf() {
		if (type != FLOAT_TYPE) throw "Invalid array data type.";

		float** float_data = (float**)data;
		float max = float_data[0][0];
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				if (float_data[i][j] > max) max = float_data[i][j];
			}
		}
		
		return max;
	}

	float Array2D::absf() {
		if (type != FLOAT_TYPE) throw "Invalid array data type.";

		float** float_data = (float**)data;
		float abs = float_data[0][0];
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				if (pow(float_data[i][j],2) > pow(abs,2)) abs = float_data[i][j];
			}
		}

		return abs;
	}

	Array2D Array2D::crossover(Array2D other_array2d, int cut_point) {
		try {
			if (type != other_array2d.type) {
				throw std::invalid_argument("The 2d arrays must have the same type");
			}
			else if (width != other_array2d.width || height != other_array2d.height) {
				throw std::invalid_argument("The arrays must have the same dimensions");
			}
		}
		catch (std::invalid_argument& e) {
			std::cout << e.what();
			exit(1);
		}

		void* new_data = NULL;

		int max_i = cut_point / width;
		int max_j = cut_point % width;

		switch (type) {
		case FLOAT_TYPE:
			new_data = malloc(height * sizeof(float*));
			for (int i = 0; i < height; i++) {
				((float**)new_data)[i] = (float*)malloc(width * sizeof(float));
				for (int j = 0; j < width; j++) {
					if (i <= max_i && j < max_j) {
						((float**)new_data)[i][j] = ((float**)data)[i][j];
					}
					else {
						((float**)new_data)[i][j] = ((float**)other_array2d.data)[i][j];
					}
				}
			}
			break;
		default:
			break;
		}
		return Array2D(FLOAT_TYPE, height, width, new_data);
	}

	Array2D Array2D::copy() {
		void* new_data = NULL;

		switch (type)
		{
		case FLOAT_TYPE:
			new_data = malloc(height * sizeof(float*));
			for (int i = 0; i < height; i++) {
				((float**)new_data)[i] = (float*)malloc(width * sizeof(float));
				for (int j = 0; j < width; j++) {
					((float**)new_data)[i][j] = ((float**)data)[i][j];
				}
			}
		default:
			break;
		}

		return Array2D(FLOAT_TYPE, height, width, new_data);
	}

	void Array2D::applyFunction(void (*function)(void*)) {
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				(*function)(&((float**)data)[i][j]);
			}
		}
	}

	Array2D Array2D::hadamardProduct(Array2D other_array2d) {
		try {
			if (type != other_array2d.type) {
				throw std::invalid_argument("The 2d arrays must have the same type");
			}
			else if (width != other_array2d.width || height != other_array2d.height) {
				throw std::invalid_argument("The arrays must have the same dimensions");
			}
		}
		catch (std::invalid_argument& e) {
			std::cout << e.what();
			exit(1);
		}

		void* new_data = NULL;

		switch (type) {
		case FLOAT_TYPE:
			new_data = malloc(height * sizeof(float*));
			for (int i = 0; i < height; i++) {
				((float**)new_data)[i] = (float*)malloc(width * sizeof(float));
				for (int j = 0; j < width; j++) {
					((float**)new_data)[i][j] = ((float**)data)[i][j] * ((float**)other_array2d.data)[i][j];
				}
			}
			break;
		}

		return Array2D(type, height, width, new_data);
	}

	Array2D Array2D::sum(Array2D other_array2d) {
		try {
			if (type != other_array2d.type) {
				throw std::invalid_argument("The 2d arrays must have the same type");
			}
			else if (width != other_array2d.width || height != other_array2d.height) {
				throw std::invalid_argument("The arrays must have the same dimensions");
			}
		}
		catch (std::invalid_argument& e) {
			std::cout << e.what();
			exit(1);
		}
		void* new_data = NULL;

		switch (type) {
		case FLOAT_TYPE:
			new_data = malloc(height * sizeof(float*));
			for (int i = 0; i < height; i++) {
				((float**)new_data)[i] = (float*)malloc(width * sizeof(float));
				for (int j = 0; j < width; j++) {
					((float**)new_data)[i][j] = ((float**)data)[i][j] + ((float**)other_array2d.data)[i][j];
				}
			}
			break;
		default:
			break;
		}

		return Array2D(type, height, width, new_data);
	}

	Array2D Array2D::dot(Array2D other_array2d) {
		try {
			if (type != other_array2d.type) {
				throw std::invalid_argument("The 2d arrays must have the same type.");
			}
			else if (width != other_array2d.height) {
				throw std::invalid_argument("Invalid dimensions array2d dot.");
			}
		}
		catch (std::invalid_argument& e) {
			std::cout << e.what();
			exit(1);
		}

		void* new_data = NULL;

		switch (type) {
		case FLOAT_TYPE:
			new_data = malloc(height * sizeof(float*));
			for (int i = 0; i < height; i++) {
				((float**)new_data)[i] = (float*)malloc(other_array2d.width * sizeof(float));
				for (int other_j = 0; other_j < other_array2d.width; other_j++) {
					((float**)new_data)[i][other_j] = 0;
					for (int j = 0; j < width; j++) {
						((float**)new_data)[i][other_j] += ((float**)data)[i][j] * ((float**)other_array2d.data)[j][other_j];
					}
				}
			}
			break;
		default:
			break;
		}

		return Array2D(type, height, other_array2d.width, new_data);
	}
}