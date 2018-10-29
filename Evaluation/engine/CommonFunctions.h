#pragma once

#include <vector>

namespace common {
	
	template <typename T> std::vector<T> concatenate(const std::vector<T>& v1, const std::vector<T>& v2) {
		std::vector<T> v;
		v.reserve(v1.size() + v2.size());
		v.insert(v.end(), v1.begin(), v1.end());
		v.insert(v.end(), v2.begin(), v2.end());
		return v;
	}

	template <typename T> void merge(std::vector<T>& v1, const std::vector<T>& v2) {
		v1.reserve(v2.size());
		v1.insert(v1.end(), v2.begin(), v2.end());
	}

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

	template <typename Base, typename Derived> void addIfUnique(std::vector<Base*>& result, const std::vector<Derived*>& elements) {
		size_t n = result.size();
		for (Derived* element : elements) {
			Base* b = element;
			if (common::indexOf<Base*>(result, element) == n) {
				result.push_back(element);
				++n;
			}
		}
	}
}