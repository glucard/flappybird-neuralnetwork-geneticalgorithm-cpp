edit0: It's an old project, and I was a complete novice in machine learning when I did it, but I'm keeping it because I like it.

edit1: converting from VStudio project to cmake project.
# flappybird-neuralnetwork-geneticalgorithm-cpp

All the codes were made by me.\
\
The goal was to implement a neural network in the game flappybird.\
\
The project creates an instance of a flappy bird game. Then, it trains a neural network using genetic algorithm.\
\
![Alt Text](./_readme/flappybird-nn.gif)


## Installation

#### install **cmake** on linux:
- `sudo apt update`
- `sudo apt install cmake`

#### Install **SFML** dependecies
- `sudo apt install libsfml-dev libx11-dev libxrandr-dev libxcursor-dev libudev-dev libfreetype-dev libopenal-devlibflac-dev libvorbis-dev ibgl1-mesa-devlibegl1-mesa-dev`

#### Use **cmake** to build the project
- Open the terminal inside the project root folder
- `cd flappybird-neuralnetwork-geneticalgorithm-cpp`
- using CMAKE to build the project:
    - `mkdir build`
    - `cd build`
    - `cmake ..`
    - `cmake --build .`
    - `./flappy_nn`

##
### In software hotkeys:
 - Space: accelerate the simulation.