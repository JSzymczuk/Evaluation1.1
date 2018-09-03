#pragma once

#include <stack>
#include <vector>
#include <algorithm>
#include "math/Math.h"

template <typename T> class SegmentTree {

public:
	SegmentTree();
	SegmentTree(int capacity);
	~SegmentTree();

	bool empty() const;
	std::vector<T> getElements() const;
	std::vector<T> broadphase(const Segment& segment) const;
	void initialize(const std::vector<T>& elements);

private:
	struct Node {
		T value;
		int parentIdx;
		int negativeChildIdx;
		int positiveChildIdx;

		bool isLeaf() const;
	};

	const static int DEFAULT_CAPACITY = 16;
	const static int NULL_ID = -1;

	int _capacity;
	int _count;
	Node* _nodes;

	int getNextIndex();
	int initializeStep(const std::vector<T>& elements, int parentIdx);
};


template <typename T> SegmentTree<T>::SegmentTree() : SegmentTree(DEFAULT_CAPACITY) {}
template <typename T> SegmentTree<T>::SegmentTree(int capacity) : _capacity(capacity), _count(0) { _nodes = new Node[capacity]; }
template <typename T> SegmentTree<T>::~SegmentTree() { delete[] _nodes; }

template <typename T> std::vector<T> SegmentTree<T>::getElements() const {
	std::vector<T> result;
	result.reserve(_count);
	for (int i = 0; i < _count; ++i) {
		result.push_back(_nodes[i].value);
	}
	return result;
}

template <typename T> bool SegmentTree<T>::Node::isLeaf() const { return positiveChildIdx != NULL_ID; }
template <typename T> bool SegmentTree<T>::empty() const { return _count == 0; }

template <typename T> std::vector<T> SegmentTree<T>::broadphase(const Segment& segment) const {

	std::vector<T> result;
	if (!empty()) {
		Vector2 from = segment.from;
		Vector2 to = segment.to;

		std::stack<int> stack;
		stack.push(0);

		while (!stack.empty()) {
			Node* current = &_nodes[stack.top()];
			stack.pop();

			Segment currentSegment = current->value.getSegment();

			int fromTriangleOrientation = common::triangleOrientation(currentSegment.from, currentSegment.to, from);
			int toTriangleOrientation = common::triangleOrientation(currentSegment.from, currentSegment.to, to);

			if (fromTriangleOrientation == toTriangleOrientation) {
				if (fromTriangleOrientation > 0) {
					if (current->positiveChildIdx != NULL_ID) { stack.push(current->positiveChildIdx); }
				}
				else if (fromTriangleOrientation < 0) {
					if (current->negativeChildIdx != NULL_ID) { stack.push(current->negativeChildIdx); }
				}
				else {
					result.push_back(current->value);
					if (current->negativeChildIdx != NULL_ID) { stack.push(current->negativeChildIdx); }
				}
			}
			else {
				result.push_back(current->value);
				if (current->positiveChildIdx != NULL_ID) { stack.push(current->positiveChildIdx); }
				if (current->negativeChildIdx != NULL_ID) { stack.push(current->negativeChildIdx); }
			}
		}
	}

	return result;
}

template <typename T> int SegmentTree<T>::getNextIndex() {
	if (_count == _capacity) {
		_capacity *= 2;
		Node* tempArray = new Node[_capacity];
		memcpy(tempArray, _nodes, sizeof(Node) * _count);
		delete[] _nodes;
		_nodes = tempArray;
	}
	return _count++;
}

template <typename T> void SegmentTree<T>::initialize(const std::vector<T>& elements) {
	if (elements.size() > 0) { initializeStep(elements, NULL_ID); }
}

template <typename T> int SegmentTree<T>::initializeStep(const std::vector<T>& elements, int parentIdx) {
	int idx = getNextIndex();
	_nodes[idx].parentIdx = parentIdx;

	if (elements.size() == 1) {
		_nodes[idx].value = elements.at(0);
		_nodes[idx].negativeChildIdx = NULL_ID;
		_nodes[idx].positiveChildIdx = NULL_ID;
	}
	else {
		int highestPriorityIdx = 0;
		int highestPriority = elements[highestPriorityIdx].getPriority();

		int n = int(elements.size());
		for (int i = 1; i < n; ++i) {
			int priority = elements[i].getPriority();
			if (priority > highestPriority) {
				highestPriorityIdx = i;
				highestPriority = priority;
			}
		}

		std::vector<T> negative;
		std::vector<T> positive;

		Segment chosenSegment = elements.at(highestPriorityIdx).getSegment();
		Vector2 from = chosenSegment.from;
		Vector2 to = chosenSegment.to;

		for (int i = 0; i < n; ++i) {

			if (i == highestPriorityIdx) { continue; }

			T element = elements.at(i);
			Segment s = element.getSegment();

			int fromTriangleOrientation = common::triangleOrientation(from, to, s.from);
			int toTriangleOrientation = common::triangleOrientation(from, to, s.to);

			if (fromTriangleOrientation > 0) {
				if (toTriangleOrientation < 0) {
					positive.push_back(element);
					negative.push_back(element);
				}
				else {
					positive.push_back(element);
				}
			}
			else if (fromTriangleOrientation < 0) {
				if (toTriangleOrientation > 0) {
					negative.push_back(element);
					positive.push_back(element);
				}
				else {
					negative.push_back(element);
				}
			}
			else if (toTriangleOrientation > 0) {
				positive.push_back(element);
			}
			else { //if (toTriangleOrientation < 0) {
				negative.push_back(element);
			}
		}

		_nodes[idx].value = elements.at(highestPriorityIdx);
		_nodes[idx].negativeChildIdx = negative.size() > 0 ? initializeStep(negative, idx) : NULL_ID;
		_nodes[idx].positiveChildIdx = positive.size() > 0 ? initializeStep(positive, idx) : NULL_ID;;
	}

	return idx;
}

template <typename T> std::vector<T> narrowphase(const Segment& segment, const std::vector<T>& potentialColliders) {
	std::vector<T> result;
	Vector2 v;
	for (T elem : potentialColliders) {
		if (common::testSegments(segment, elem.getSegment(), v)) {
			result.push_back(elem);
		}
	}
	return result;
}