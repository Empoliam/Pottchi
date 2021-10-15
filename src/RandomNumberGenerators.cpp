#include "./headers/RandomNumberGenerators.h"

#include <random>
#include <chrono>

unsigned long long seed = std::chrono::system_clock::now().time_since_epoch().count();
static std::default_random_engine randGen((int)seed);
static std::uniform_real_distribution<float> rUnif(0.0f, 1.0f);

float RandomNumberGenerators::rUnifProb()
{
	return rUnif(randGen);
}

int RandomNumberGenerators::rUnifInt(int min, int max) {

	std::uniform_int_distribution<int> rInt(min, max);
	return rInt(randGen);

}