#pragma once

#include <vector>
#include "main//Configuration.h"
#include "entities/Trigger.h"

class TriggerFactory {
public:
	static Trigger* create(TriggerType type, const Vector2& position);
	static Trigger* create(const String& type, const Vector2& position);

	static void initialize();
	static void destroyAll();

private:
	static std::vector<Trigger*> _triggers;
	static int _created[TRIGGERS_SIZE];
	static bool _initialized;
};
