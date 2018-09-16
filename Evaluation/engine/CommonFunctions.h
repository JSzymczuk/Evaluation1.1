#pragma once

#include <vector>

namespace common {

	template <typename T> bool contains(const std::vector<T>& vec, T element) {
		size_t n = vec.size();
		for (size_t i = 0; i < n; ++i) {
			if (vec.at(i) == element) {
				return true;
			}
		}
		return false;
	}

	template <typename T> size_t indexOf(const std::vector<T>& vec, T element) {
		size_t n = vec.size();
		for (size_t i = 0; i < n; ++i) {
			if (vec.at(i) == element) {
				return i;
			}
		}
		return n;
	}

	template <typename T> void swapLastAndRemove(std::vector<T>& vec, size_t idx) {
		size_t n = vec.size();
		if (n > 0) {
			if (idx < n - 1) {
				vec[idx] = vec.at(n - 1);
			}
			vec.pop_back();
		}
	}

	template <typename T> void addIfUnique(std::vector<T>& result, const std::vector<T>& elements) {
		size_t n = result.size();
		for (T element : elements) {
			if (common::indexOf(result, element) == n) {
				result.push_back(element);
				++n;
			}
		}
	}
}