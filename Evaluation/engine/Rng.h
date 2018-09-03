#pragma once
#include <random>

class Rng {

private:
	static std::default_random_engine generator;
	static std::uniform_int_distribution<int> uid;
	static std::uniform_real_distribution<float> urd;
	static std::normal_distribution<float> nd;

public:
	static int getInteger(int minVal, int maxVal);
	static float getFloat(float minVal, float maxVal);
	static float getFloatNormal();
};
