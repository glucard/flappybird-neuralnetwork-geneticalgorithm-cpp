#ifndef _FLAPPY_BIRD_INTERFACE_H_
#define _FLAPPY_BIRD_INTERFACE_H_

#include "FlappyBirdLib/FlappyBird.h"
#include "NeuralNetworkLib/NeuralNetwork.h"
#include "GeneticAlgorithmLib/GeneticAlgorithm.h"

#include "SFML/Graphics.hpp"
#include <Windows.h>
#include <string>
#include <iostream>
#include <sstream>

#define RESOLUTION_X 800
#define RESOLUTION_Y 600
#define GRAVITY_ACCELERATION 0.5
#define TUNNEL_VELOCITY -2.5
#define BIRD_FLAP_ACCELERATION 7
#define BIRD_MAX_SPEED 17
#define TICKETS 30

namespace FlappyBirdInterce {
	void drawInterfaceGA(GeneticAlgorithm::GeneticAlgorithm* ga);
}

#endif // !_FLAPPY_BIRD_INTERFACE_H_