#include "Rng.h"

std::default_random_engine Rng::generator;
std::uniform_int_distribution<int> Rng::uid;
std::uniform_real_distribution<float> Rng::urd;
std::normal_distribution<float> Rng::nd = std::normal_distribution<float>(0.0f, 0.5f);

int Rng::getInteger(int minVal, int maxVal) {
	return uid(generator, std::uniform_int_distribution<int>::param_type{ minVal, maxVal });
}

float Rng::getFloat(float minVal, float maxVal) {
	return urd(generator, std::uniform_real_distribution<float>::param_type{ minVal, maxVal });
}

float Rng::getFloatNormal() {
	return nd(generator);
}