#pragma once

class RandomNumberGenerators {

public:

	static double rUnifProb();
	static int rUnifInt(int min, int max);
	static double rNormalDouble(double mu, double sdev);

private:

	RandomNumberGenerators() {}

};
