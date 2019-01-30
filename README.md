# AIMaze2

## Brief Description
AIMaze2 is a study project about machine learning techniques. In particular, a NEAT algorithm is used in order to evolve neural networks.

### Game Rules
The AI plays the [Google Chrome Dino Game](https://en.wikipedia.org/wiki/List_of_Google_Easter_eggs#Chrome).

![ScreenShot Game](https://github.com/BiagioFesta/aimaze2/blob/master/img/screenshotJan19.png)

The player can perform two actions: either jumping or bowing in order to avoid the obstacles during the run. The dino will bump into different kinds of obstacles each of them has different shape and position.

### AI Evolution
The evolution of the AI is performed by means of a NEAT ([Neuroevolution of augmenting topologies](https://en.wikipedia.org/wiki/Neuroevolution_of_augmenting_topologies)) algorithm.
It allows evolution and learning of the neural network by means of a genetic algorithm.

In the beginning, the network is entirely lacking connections. Only input and output neurons are instantiated.
Subsequently, a population of several networks will start playing the game and learning with the classic selection and crossover typical of genetic algorithms. The score performed during each generation will give the value indicative of the network's goodness.


## Technical Aspects
 * As this project is primarily an educative study, all methodologies and the environment are implemented and written from scratch.
 * The software is entirely written in C++. In particular, the new standard C++17.
