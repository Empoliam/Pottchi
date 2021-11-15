#pragma once

class RandomNumberGenerators {

public:

	static float rUnifProb();
	static int rUnifInt(int min, int max);
	static float rNormalFloat(float mu, float sdev);

private:

	RandomNumberGenerators() {}

};
