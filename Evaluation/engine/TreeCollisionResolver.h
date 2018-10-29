#include "engine/CollisionResolver.h"
#include "engine/AabbTree.h"

class TreeCollisionResolver : public CollisionResolver {
public:
	TreeCollisionResolver();
	~TreeCollisionResolver();

	void initializeDynamic(const std::vector<DynamicEntity*>& dynamicObjects) override;

	void add(DynamicEntity* element) override;
	void add(StaticEntity* element) override;
	void remove(DynamicEntity* element) override;
	void update(Movable* element) override;

	std::vector<DynamicEntity*> broadphaseDynamic(const Vector2& point, float radius) const override;
	std::vector<DynamicEntity*> broadphaseDynamic(const Vector2& from, const Vector2& to) const override;
	std::vector<DynamicEntity*> broadphaseDynamic(const Vector2& from, const Vector2& to, float radius) const override;

	std::vector<StaticEntity*> broadphaseStatic(const Vector2& point, float radius) const override;
	std::vector<StaticEntity*> broadphaseStatic(const Vector2& from, const Vector2& to) const override;
	std::vector<StaticEntity*> broadphaseStatic(const Vector2& from, const Vector2& to, float radius) const override;

	std::vector<Aabb> getDynamicAabbs() const;
	std::vector<Aabb> getStaticAabbs() const;

private:
	std::vector<StaticEntity*> _staticToAdd;
	AabbTree<StaticEntity*> _static;
	AabbTree<DynamicEntity*> _dynamic;
};