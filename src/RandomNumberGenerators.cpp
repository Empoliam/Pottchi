#include "./headers/RandomNumberGenerators.h"

#include <chrono>
#include <random>

unsigned long long seed = std::chrono::system_clock::now().time_since_epoch().count();

static std::default_random_engine randGen((int)seed);
static std::uniform_real_distribution<double> rUnif(0.0f, 1.0f);

/**
 * @brief Generate a random uniform number between 0.0 and 1.0
 *
 * @return double
 */
double RandomNumberGenerators::rUnifProb() {
	return rUnif(randGen);
}

/**
 * @brief Generate a random uniform integer between min and max
 *
 * @param min
 * @param max
 * @return int
 */
int RandomNumberGenerators::rUnifInt(int min, int max) {

	std::uniform_int_distribution<int> rInt(min, max);
	return rInt(randGen);
}

/**
 * @brief Generate a random normally distributed double
 *
 * @param mu Mean
 * @param stdev Standard deviation
 * @return double
 */
double RandomNumberGenerators::rNormalDouble(double mu, double stdev) {

	std::normal_distribution<double> rNorm(mu, stdev);
	return rNorm(randGen);
}