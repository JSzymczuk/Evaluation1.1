#pragma once

#include <algorithm>
#include <stack>
#include <vector>
#include <mutex>
#include "math/Aabb.h"

template <typename Item> class AabbTree {

public:
	static const int DEFAULT_CAPACITY;
	static const float DEFAULT_LEAF_MARGIN;
private:
	static const int NULL_ID;

private:
	struct AabbNode {
		Item value;
		Aabb aabb;
		int parentId;
		int leftChildId;
		int rightChildId;

		AabbNode();
		bool isLeaf() const;
	};

private:
	AabbNode * _nodes;
	size_t _capacity;
	size_t _count;
	size_t _rootId;
	float _leafMargin;
	std::mutex mutex;

public:
	AabbTree();
	AabbTree(float margin);
	AabbTree(float margin, int capacity);
	~AabbTree();

	// Czy drzewo jest puste.
	bool isEmpty() const;

	// Zwraca wszystkie Aabb wchodz¹ce w sk³ad drzewa.
	std::vector<Aabb> getAabbs() const;

	// Zwraca wszystkie elementy znajduj¹ce siê w drzewie.
	std::vector<Item> getElements() const;

	// Tworzy drzewo Aabb metod¹ top-down z podanych elementów. Z³o¿onoœæ O(nlogn).
	static void initialize(AabbTree<Item>* tree, const std::vector<Item>& elements);

	// Tworzy drzewo Aabb metod¹ top-down z podanych elementów. Z³o¿onoœæ O(nlogn).
	static void initialize(AabbTree<Item>* tree, const std::vector<Item>& elements, float leafMargin);

	// Niszczy drzewo.
	static void dispose(AabbTree<Item>* tree);

	// Dodaje element do drzewa. Z³o¿onoœæ O(logn).
	void insert(Item item);

	// Sprawdza czy element wystêpuje w drzewie. Pesymistyczna z³o¿onoœæ O(n).
	bool contains(Item item) const;

	// Usuwa element z drzewa. Pesymistyczna z³o¿onoœæ O(n).
	void remove(Item item);

	// Uaktualnia po³o¿enie elementu w drzewie. Pesymistyczna z³o¿onoœæ O(n).
	void update(Item item);

	// Usuwa wszystkie elementy z drzewa. Z³o¿onoœæ O(n).
	void clear();

	// Przeprowadza wstêpn¹ selekcjê elementów, które mog¹ kolidowaæ z podanym. Pesymistyczna z³o¿onoœæ: O(logn).
	std::vector<Item> broadphase(const Vector2& point) const;

	// Przeprowadza wstêpn¹ selekcjê elementów, które mog¹ kolidowaæ z podanym. Pesymistyczna z³o¿onoœæ: O(logn).
	std::vector<Item> broadphase(const Aabb& aabb) const;

private:
	// Ustawia domyœlne wartoœci wêz³a.
	void clearNode(int index);

	// Ustawia wartoœci wêz³a.
	void setNode(size_t index, Item value, Aabb aabb);

	// Ustawia wartoœci wêz³a.
	void setNode(size_t index, Item value, Aabb aabb, int parentId, int leftChildId, int rightChildId);

	// Przydziela miejsce w tablicy wêz³ów. 
	// Jeœli nie jest dostêpne, poszerza tablicê. 
	// Zwraca indeks przydzielonego wêz³a.
	int getNextIndex();

	// Kasuje wêze³ na podanej pozycji. Wywo³ywane przez remove(item). Nie sprawdza warunków. Z³o¿onoœæ O(1).
	void removeAt(int nodeId);

	// Zamienia wêz³y miejscami i poprawia odpowiednie zale¿noœci. Nie sprawdza warunków. Z³o¿onoœæ O(1).
	void swapNodes(int index1, int index2);

	// Metoda pomocnicza do RemoveAt(index). Zak³ada, ¿e na ostaniej pozycji
	// znajduje siê liœæ, a na przedostatniej jego rodzic, które maj¹ zostaæ skasowane.
	// Nie sprawdza warunków. Z³o¿onoœæ O(1).
	void removeLastTwoNodes();

	// Sprawdza czy element wystêpuje w drzewie. Jeœli tak, zwraca jego indeks. Pesymistyczna z³o¿onoœæ O(n).
	bool contains(Item item, int& index) const;


private:
	class AabbTreeBuilder {

	public:
		void initialize(AabbTree<Item>* tree, const std::vector<Item>& data, float leafMargin);

	private:
		enum SplitType {
			X, Y
		};

		struct ElementData {
			Item value;
			Aabb aabb;
			Vector2 center;

			ElementData() : aabb(Aabb(0, 0, 0, 0)) {}
			float& operator[](SplitType type) {
				return type == SplitType::X ? center.x : center.y;
			}
		};

		int split(ElementData* data, SplitType splitType, float centerValue, int from, int to);
		int constructStep(ElementData* data, int from, int to);
		Aabb constructAabb(ElementData* data, int from, int to);
		template<typename T> void swap(T* array, int index1, int index2);

		int nodesCreated;
		AabbNode* _nodes;
	};

};

template <typename Item> const int AabbTree<Item>::DEFAULT_CAPACITY = 16;
template <typename Item> const float AabbTree<Item>::DEFAULT_LEAF_MARGIN = 1.1f;
template <typename Item> const int AabbTree<Item>::NULL_ID = -1;

template<typename T> T* getPointer(T& obj) { return &obj; }
template<typename T> T* getPointer(T* obj) { return obj; }


template<typename Item> AabbTree<Item>::AabbNode::AabbNode() : aabb(Aabb(0, 0, 0, 0)) { }
template<typename Item> bool AabbTree<Item>::AabbNode::isLeaf() const { return leftChildId == NULL_ID; }

template<typename Item> AabbTree<Item>::AabbTree() : AabbTree(DEFAULT_LEAF_MARGIN, DEFAULT_CAPACITY) { }

template<typename Item> AabbTree<Item>::AabbTree(float margin) : AabbTree(margin, DEFAULT_CAPACITY) { }

template<typename Item> AabbTree<Item>::AabbTree(float margin, int capacity)
	: _rootId(NULL_ID), _leafMargin(margin), _capacity(capacity), _count(0) {
	_nodes = new AabbNode[capacity];
}

template<typename Item> AabbTree<Item>::~AabbTree() { 
	delete[] _nodes; 
}

template<typename Item> bool AabbTree<Item>::isEmpty() const { return _rootId == NULL_ID; }

template<typename Item> void AabbTree<Item>::insert(Item item) {
	mutex.lock();

	// Przydzielamy miejsce w tablicy i przygotowujemy aabb.
	int newNodeId = getNextIndex();
	auto itemPointer = getPointer(item);
	Aabb colliderAabb = itemPointer->getAabb();
	if (!itemPointer->isStaticElement()) {
		colliderAabb.scale(_leafMargin);
	}

	// Jeœli dodajemy pierwszy wêze³, to staje siê on korzeniem i wychodzimy.
	if (isEmpty()) {
		_rootId = newNodeId;
		setNode(_rootId, item, colliderAabb);
		mutex.unlock();
		return;
	}

	// W przeciwnym razie schodzimy w dó³ drzewa a¿ dotrzemy do optymalnego liœcia
	// (czyli tego, dla którego nast¹pi najmniejszy przyrost niewykorzystanej powierzchni)
	int tempNodeId = _rootId;
	while (!_nodes[tempNodeId].isLeaf()) {
		int leftId = _nodes[tempNodeId].leftChildId;
		int rightId = _nodes[tempNodeId].rightChildId;

		Aabb tempAabb = _nodes[leftId].aabb;
		float diffLeft = Aabb::merge(colliderAabb, tempAabb).getVolume()
			- tempAabb.getVolume();
		tempAabb = _nodes[rightId].aabb;
		float diffRight = Aabb::merge(colliderAabb, tempAabb).getVolume()
			- tempAabb.getVolume();

		if (diffLeft < diffRight) {
			tempNodeId = leftId;
		}
		else {
			tempNodeId = rightId;
		}
	}

	// Dodajemy liœæ. Znaleziony liœæ staje siê bratem nowego 
	// liœcia, a ich rodzicem jest nowo utworzony wêze³.
	int newparentId = getNextIndex();
	int parentId = _nodes[tempNodeId].parentId;

	setNode(newparentId, Item(), Aabb::merge(colliderAabb,
		_nodes[tempNodeId].aabb), parentId, tempNodeId, NULL_ID);
	_nodes[tempNodeId].parentId = newparentId;
	setNode(newNodeId, item, colliderAabb, newparentId, NULL_ID, NULL_ID);
	_nodes[newparentId].rightChildId = newNodeId;

	// Wspinamy siê w górê drzewa i uaktualniamy aabb przodków.
	if (parentId != NULL_ID) {
		if (_nodes[parentId].leftChildId == tempNodeId) {
			_nodes[parentId].leftChildId = newparentId;
		}
		else {
			_nodes[parentId].rightChildId = newparentId;
		}

		while (parentId != NULL_ID) {
			AabbNode parent = _nodes[parentId];
			_nodes[parentId].aabb = Aabb::merge(_nodes[parent.leftChildId].aabb,
				_nodes[parent.rightChildId].aabb);
			parentId = parent.parentId;
		}
	}
	else {
		_rootId = newparentId;
	}

	mutex.unlock();
}

template<typename Item> bool AabbTree<Item>::contains(Item item) const {
	int index;
	return contains(item, index);
}

template<typename Item> void AabbTree<Item>::remove(Item item) {
	mutex.lock();
	int index;
	if (contains(item, index)) {
		removeAt(index);
	}
	mutex.unlock();

	//for (int i = 0; i < _count; ++i) {
	//	AabbNode& node = _nodes[i];
	//	if (node.parentId != NULL_ID) {
	//		if (_nodes[node.parentId].leftChildId != i && _nodes[node.parentId].rightChildId != i) { throw; }
	//		if (!_nodes[node.parentId].aabb.contains(node.aabb)) { throw; }
	//	}
	//	if (node.leftChildId != NULL_ID) {
	//		if (_nodes[node.leftChildId].parentId != i) { throw; }
	//	}
	//	if (node.rightChildId != NULL_ID) {
	//		if (_nodes[node.rightChildId].parentId != i) { throw; }
	//	}
	//}
}

template<typename Item> void AabbTree<Item>::update(Item item) {
	int index;
	if (contains(item, index) && !_nodes[index].aabb.contains(item->getAabb())) {
		mutex.lock();
		removeAt(index);
		mutex.unlock();
		insert(item);
	}
}

template<typename Item> void AabbTree<Item>::clear() {
	mutex.lock();
	for (int i = 0; i < count; ++i) {
		clearNode(i);
	}
	count = 0;
	rootId = NULL_ID;
	mutex.unlock();
}

template<typename Item> std::vector<Aabb> AabbTree<Item>::getAabbs() const {
	std::vector<Aabb> result = std::vector<Aabb>();
	result.reserve(_count);
	for (int i = 0; i < _count; i++) {
		result.push_back(_nodes[i].aabb);
	}
	return result;
}

template<typename Item> std::vector<Item> AabbTree<Item>::getElements() const {
	std::vector<Item> result = std::vector<Item>();
	result.reserve(_count / 2);
	for (int i = 0; i < _count; i++) {
		if (_nodes[i].isLeaf()) {
			result.push_back(_nodes[i].value);
		}
	}
	return result;
}

template<typename Item> void AabbTree<Item>::initialize(AabbTree<Item>* tree, const std::vector<Item>& elements) {
	return initialize(tree, elements, DEFAULT_LEAF_MARGIN);
}

template<typename Item> void AabbTree<Item>::initialize(AabbTree<Item>* tree, const std::vector<Item>& elements, float leafMargin) {
	return AabbTreeBuilder().initialize(tree, elements, leafMargin);
}

template<typename Item> void AabbTree<Item>::dispose(AabbTree<Item>* tree) { delete tree; }

template<typename Item> std::vector<Item> AabbTree<Item>::broadphase(const Vector2& point) const {
	std::vector<Item> result = std::vector<Item>();
	if (!isEmpty()) {
		std::stack<int> stack = std::stack<int>();
		stack.push(_rootId);

		while (!stack.empty()) {
			auto temp = _nodes[stack.top()];
			stack.pop();

			if (temp.aabb.contains(point)) {
				if (temp.isLeaf()) {
					result.push_back(temp.value);
				}
				else {
					stack.push(temp.leftChildId);
					stack.push(temp.rightChildId);
				}
			}
		}
	}
	return result;
}

template<typename Item> std::vector<Item> AabbTree<Item>::broadphase(const Aabb& aabb) const {
	std::vector<Item> result = std::vector<Item>();
	if (!isEmpty()) {
		std::stack<int> stack = std::stack<int>();
		stack.push(_rootId);

		while (!stack.empty()) {
			auto temp = _nodes[stack.top()];
			stack.pop();

			if (temp.aabb.intersects(aabb)) {
				if (temp.isLeaf()) {
					result.push_back(temp.value);
				}
				else {
					stack.push(temp.leftChildId);
					stack.push(temp.rightChildId);
				}
			}
		}
	}
	return result;
}

template<typename Item> void AabbTree<Item>::clearNode(int index) {
	_nodes[index].value = default(Item);
	_nodes[index].parentId = NULL_ID;
	_nodes[index].leftChildId = NULL_ID;
	_nodes[index].rightChildId = NULL_ID;
}

template<typename Item> void AabbTree<Item>::setNode(size_t index, Item value, Aabb aabb) {
	setNode(index, value, aabb, NULL_ID, NULL_ID, NULL_ID);
}

template<typename Item> void AabbTree<Item>::setNode(size_t index, Item value, Aabb aabb, int parentId, int leftChildId, int rightChildId) {
	AabbNode node = AabbNode();
	node.value = value;
	node.aabb = aabb;
	node.parentId = parentId;
	node.leftChildId = leftChildId;
	node.rightChildId = rightChildId;
	_nodes[index] = node;
}

template<typename Item> int AabbTree<Item>::getNextIndex() {
	if (_count == _capacity) {
		_capacity *= 2;
		AabbNode* tempArray = new AabbNode[_capacity];
		memcpy(tempArray, _nodes, sizeof(AabbNode) * _count);
		delete[] _nodes;
		_nodes = tempArray;
	}
	return _count++;
}

template<typename Item> void AabbTree<Item>::removeAt(int nodeId) {
	// Jeœli indeks kasowanego wêz³a jest indeksem korzenia, to musi byæ jedynym liœciem.
	if (nodeId == _rootId) {
		_rootId = NULL_ID;
		_count = 0;
		return;
	} // ...w przeciwnym razie parentId != NULL_ID.

	  // Jeœli dotarliœmy do tego miejsca, to istniej¹ przynajmniej 3 wêz³y.
	int parentId = _nodes[nodeId].parentId;

	if (parentId != _count - 2) {
		swapNodes(parentId, _count - 2);

		if (nodeId != _count - 1)
		{
			if (nodeId != _count - 2)
			{
				swapNodes(nodeId, _count - 1);
			}
			else
			{
				swapNodes(parentId, _count - 1);
			}
		}
	}
	else if (nodeId != _count - 1) {
		swapNodes(nodeId, _count - 1);
	}

	removeLastTwoNodes();
}

template<typename Item> void AabbTree<Item>::swapNodes(int index1, int index2) {
	if (index1 == index2) return;

	AabbNode n1 = _nodes[index1];
	AabbNode n2 = _nodes[index2];

	_nodes[index1] = n2;
	_nodes[index2] = n1;

	if (n1.parentId != NULL_ID) {
		if (n1.parentId != index2) {
			if (_nodes[n1.parentId].leftChildId == index1) {
				_nodes[n1.parentId].leftChildId = index2;
			}
			else {
				_nodes[n1.parentId].rightChildId = index2;
			}
		}
		else {
			_nodes[index2].parentId = index1;
			if (n2.leftChildId == index1) {
				_nodes[index1].leftChildId = index2;
			}
			else {
				_nodes[index1].rightChildId = index2;
			}
		}
	}
	else { _rootId = index2; }

	if (n2.parentId != NULL_ID) {
		if (n2.parentId != index1) {
			if (_nodes[n2.parentId].leftChildId == index2) {
				_nodes[n2.parentId].leftChildId = index1;
			}
			else {
				_nodes[n2.parentId].rightChildId = index1;
			}
		}
		else {
			_nodes[index1].parentId = index2;
			if (n1.leftChildId == index2) {
				_nodes[index2].leftChildId = index1;
			}
			else {
				_nodes[index2].rightChildId = index1;
			}
		}
	}
	else {
		_rootId = index1;
	}

	if (!n1.isLeaf()) {
		_nodes[_nodes[index2].leftChildId].parentId = index2;
		_nodes[_nodes[index2].rightChildId].parentId = index2;
	}

	if (!n2.isLeaf()) {
		_nodes[_nodes[index1].leftChildId].parentId = index1;
		_nodes[_nodes[index1].rightChildId].parentId = index1;
	}
}


template<typename Item> void AabbTree<Item>::removeLastTwoNodes() {
	int leafId = --_count;
	int branchId = --_count;
	// Kasujemy rodzica.
	int siblingId = _nodes[branchId].leftChildId == leafId ?
		_nodes[branchId].rightChildId : _nodes[branchId].leftChildId;
	int parentId = _nodes[branchId].parentId;
	if (parentId != NULL_ID) {
		if (_nodes[parentId].leftChildId == branchId) {
			_nodes[parentId].leftChildId = siblingId;
		}
		else {
			_nodes[parentId].rightChildId = siblingId;
		}
		_nodes[siblingId].parentId = parentId;

		while (parentId != NULL_ID) {
			_nodes[parentId].aabb = Aabb::merge(
				_nodes[_nodes[parentId].rightChildId].aabb,
				_nodes[_nodes[parentId].leftChildId].aabb);
			parentId = _nodes[parentId].parentId;
		}
	}
	else {
		// Jeœli nie by³o rodzica, to by³ korzeniem.
		_nodes[siblingId].parentId = NULL_ID;
		_rootId = siblingId;
	}
}

template<typename Item> bool AabbTree<Item>::contains(Item item, int& index) const {
	if (this->isEmpty()) {
		index = NULL_ID;
		return false;
	}

	Aabb itemAabb = getPointer(item)->getAabb();
	std::stack<int> stack = std::stack<int>();
	stack.push(_rootId);

	while (stack.size() > 0) {
		int tempId = stack.top();
		stack.pop();
		auto& temp = _nodes[tempId];
		if (temp.aabb.intersects(itemAabb)) {
			if (temp.isLeaf()) {
				if (temp.value == item) {
					index = tempId;
					return true;
				}
			}
			else {
				stack.push(temp.leftChildId);
				stack.push(temp.rightChildId);
			}
		}
	}

	index = NULL_ID;
	return false;
}


template<typename Item> void AabbTree<Item>::AabbTreeBuilder::initialize(AabbTree<Item>* tree, const std::vector<Item>& data, float leafMargin) {
	
	if (tree->_nodes != nullptr) {
		delete[] tree->_nodes;
	}
	nodesCreated = 0;
	size_t size = data.size();

	if (size != 0) {
		// Przygotowanie do przetwarzania
		_nodes = new AabbNode[2 * size - 1];

		auto nodeData = new ElementData[size];
		for (int i = 0; i < size; ++i) {
			auto element = data.at(i);
			auto elementPointer = getPointer(element);
			Aabb aabb = elementPointer->getAabb();
			if (!elementPointer->isStaticElement()) {
				aabb.scale(leafMargin);
			}

			nodeData[i].value = element;
			nodeData[i].aabb = aabb;
			nodeData[i].center = aabb.getCenter();
		}

		tree->_rootId = constructStep(nodeData, 0, size - 1);
		tree->_capacity = nodesCreated;
	}
	else { 
		_nodes = new AabbNode[DEFAULT_CAPACITY]; 
		tree->_capacity = DEFAULT_CAPACITY;
		tree->_rootId = NULL_ID;
	}

	tree->_leafMargin = leafMargin;
	tree->_nodes = _nodes;
	tree->_count = nodesCreated;
}

template<typename Item> int AabbTree<Item>::AabbTreeBuilder::split(ElementData* data, SplitType splitType, float centerValue, int from, int to) {
	int i = from;
	while (i < to) {
		if (data[i][splitType] < centerValue) {
			++i;
		}
		else {
			swap(data, i, to);
			--to;
		}
	}
	return data[i][splitType] < centerValue ? to + 1 : to;
}

template<typename Item> int AabbTree<Item>::AabbTreeBuilder::constructStep(ElementData* data, int from, int to) {
	if (from == to) {
		// Dotarliœmy do liœcia.
		_nodes[nodesCreated].value = data[from].value;
		_nodes[nodesCreated].aabb = data[from].aabb;
		_nodes[nodesCreated].parentId = NULL_ID;
		_nodes[nodesCreated].leftChildId = NULL_ID;
		_nodes[nodesCreated].rightChildId = NULL_ID;
	}
	else {
		// W przeciwnym wypadku rozwa¿amy obszar p³aszczyzny, 
		// który bêdziemy nadal dzieliæ na czêœci.
		Aabb aabb = constructAabb(data, from, to);
		int splitIdx;
		SplitType splitType;
		float splitValue;
		// Dzielimy zawsze d³u¿szy bok.
		if (aabb.getHeight() > aabb.getWidth()) {
			splitType = SplitType::Y;
			splitValue = aabb.getCenter().y;
		}
		else {
			splitType = SplitType::X;
			splitValue = aabb.getCenter().x;
		}
		// Split rozstrzyga przynale¿noœæ do czêœci: [from; center) [center; to].
		// Druga czêœæ zawsze zawieraæ bêdzie przynajmniej jeden element.
		// Mo¿e wyst¹piæ sytuacja, w której wszystkie elementy znajd¹ siê w drugiej czêœci.
		// Wówczas powtarzamy proces podzia³u wzglêdem punktu (center + to) / 2.
		do {
			splitIdx = split(data, splitType, splitValue, from, to);
			splitValue = (splitValue + data[to][splitType]) / 2;
		} while (splitIdx > to);

		AabbNode branchNode = AabbNode();
		branchNode.aabb = aabb;
		branchNode.leftChildId = constructStep(data, from, splitIdx - 1);
		branchNode.rightChildId = constructStep(data, splitIdx, to);
		branchNode.parentId = NULL_ID;

		_nodes[nodesCreated] = branchNode;
		_nodes[branchNode.leftChildId].parentId = nodesCreated;
		_nodes[branchNode.rightChildId].parentId = nodesCreated;
	}
	return nodesCreated++;
}

template<typename Item> Aabb AabbTree<Item>::AabbTreeBuilder::constructAabb(ElementData* data, int from, int to) {
	Aabb aabb = data[from].aabb;
	Vector2 min = aabb.getTopLeft();
	Vector2 max = aabb.getBottomRight();

	for (int i = from + 1; i <= to; ++i) {
		aabb = data[i].aabb;
		min = Vector2::min(min, aabb.getTopLeft());
		max = Vector2::max(max, aabb.getBottomRight());
	}

	return Aabb(min, max);
}

template<typename Item> template<typename T> void AabbTree<Item>::AabbTreeBuilder::swap(T* array, int index1, int index2) {
	T temp = array[index1];
	array[index1] = array[index2];
	array[index2] = temp;
}