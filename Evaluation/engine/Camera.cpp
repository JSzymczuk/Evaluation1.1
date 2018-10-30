#include "main/Configuration.h"
#include "math/Math.h"
#include <SDL_image.h>
#include "SDL_ttf.h"
#include "entities/Trigger.h"
#include "entities/Actor.h"
#include "ResourceManager.h"
#include "engine/Camera.h"
#include "entities/Team.h"

Camera::Camera(int x, int y, int xMin, int xMax, int yMin, int yMax)
	: _xMin(xMin), _xMax(xMax), _yMin(yMin), _yMax(yMax) {
	_x = x < xMin ? xMin : x > xMax ? xMax : x;
	_y = y < yMin ? yMin : y > yMax ? yMax : y;
	_speed = Config.CameraSpeed > 0 ? Config.CameraSpeed : 1;
}

void Camera::update(bool up, bool right, bool down, bool left) {
	if (left && !right) {
		_x -= _speed;
		if (_x < _xMin) { _x = _xMin; }
	}
	else if (right && !left) {
		_x += _speed;
		if (_x > _xMax) { _x = _xMax; }
	}
	if (up && !down) {
		_y -= _speed;
		if (_y < _yMin) { _y = _yMin; }
	}
	else if (down && !up) {
		_y += _speed;
		if (_y > _yMax) { _y = _yMax; }
	}
}

// screen = world - camera
SDL_Rect Camera::getRenderArea(int x, int y, int width, int height) const {
	return { x - _x, y - _y, width, height };
}

SDL_Rect Camera::getRenderArea(float x, float y, float width, float height) const {
	return { int(x - _x), int(y - _y), int(width), int(height) };
}

void Camera::adjustRenderArea(SDL_Rect& rect) const {
	rect.x -= _x;
	rect.y -= _y;
}

int Camera::getX() const { return _x; }

int Camera::getY() const { return _y; }

void drawTexture(SDL_Renderer* renderer, SDL_Texture* texture, const Vector2& position, const Camera& camera, PositionType positionType) {
	int width, height;
	SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);
	SDL_Rect tRect = { int(position.x), int(position.y), width, height };
	if (positionType == PositionType::Relative) { camera.adjustRenderArea(tRect); }
	SDL_RenderCopy(renderer, texture, nullptr, &tRect);
}

void drawTexture(SDL_Renderer* renderer, SDL_Texture* texture, const Vector2& center, const Camera& camera, PositionType positionType, float orientation, bool horizontalFlip) {
	int width, height;
	SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);
	int widthHalf = width / 2;
	int heightHalf = height / 2;
	SDL_Rect tRect{ int(center.x) - widthHalf, int(center.y) - heightHalf, width, height };
	if (positionType == PositionType::Relative) { camera.adjustRenderArea(tRect); }
	SDL_Point tCenter = { widthHalf, heightHalf };
	SDL_RenderCopyEx(renderer, texture, nullptr, &tRect, common::degrees(orientation), &tCenter,
		horizontalFlip ? SDL_RendererFlip::SDL_FLIP_HORIZONTAL : SDL_RendererFlip::SDL_FLIP_NONE);
}

void drawString(SDL_Renderer* renderer, const char* string, int x, int y, const Camera& camera, PositionType positionType, bool centered, const SDL_Color& color) {
	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(ResourceManager::get()->getFont("mainfont"), string, color);
	SDL_Texture* message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
	int w = surfaceMessage->w, h = surfaceMessage->h;
	SDL_Rect messageRect{ x, y, w, h };
	if (centered) {
		messageRect.x -= w / 2;
		messageRect.y -= h / 2;
	}
	if (positionType == PositionType::Relative) { camera.adjustRenderArea(messageRect); }
	SDL_RenderCopy(renderer, message, nullptr, &messageRect);
	SDL_DestroyTexture(message);
	SDL_FreeSurface(surfaceMessage);
}

void drawAabb(SDL_Renderer* renderer, const Aabb& aabb, const Camera& camera, const SDL_Color& color) {
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	SDL_Rect rect{ int(aabb.getLeft()), int(aabb.getTop()), int(aabb.getWidth()), int(aabb.getHeight()) };
	camera.adjustRenderArea(rect);
	SDL_RenderDrawRect(renderer, &rect);
}

void fillAabb(SDL_Renderer* renderer, const Aabb& aabb, const Camera& camera, const SDL_Color& color) {
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	SDL_Rect rect{ int(aabb.getLeft()), int(aabb.getTop()), int(aabb.getWidth()), int(aabb.getHeight()) };
	camera.adjustRenderArea(rect);
	SDL_RenderFillRect(renderer, &rect);
}

void drawSegment(SDL_Renderer* renderer, const Segment& segment, const Camera& camera, const SDL_Color& color) {
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	int x = camera.getX(), y = camera.getY();
	SDL_RenderDrawLine(renderer, int(segment.from.x) - x, int(segment.from.y) - y, int(segment.to.x) - x, int(segment.to.y) - y);
}

void drawPoint(SDL_Renderer* renderer, const Vector2& point, const Camera& camera, const SDL_Color& color) {
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	SDL_Rect rect = { int(point.x - 2), int(point.y - 2), 5, 5 };
	camera.adjustRenderArea(rect);
	SDL_RenderFillRect(renderer, &rect);
}

void drawCircle(SDL_Renderer* renderer, const Vector2& center, float radius, const Camera& camera, const SDL_Color& color) {
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	int x = int(center.x) - camera.getX(), y = int(center.y) - camera.getY(), r = int(radius), r2 = r * r;
	for (int i = -r; i <= r; ++i) {
		int t = sqrt(r2 - i * i);
		SDL_RenderDrawPoint(renderer, x - t, y + i);
		SDL_RenderDrawPoint(renderer, x + t, y + i);
	}
}

void fillCircle(SDL_Renderer* renderer, const Vector2& center, float radius, const Camera& camera, const SDL_Color& color) {
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	int x = int(center.x) - camera.getX(), y = int(center.y) - camera.getY(), r = int(radius), r2 = r * r;
	for (int i = -r; i <= r; ++i) {
		int t = sqrt(r2 - i * i);
		SDL_RenderDrawLine(renderer, x - t, y + i, x + t, y + i);
	}
}

void fillRing(SDL_Renderer* renderer, const Vector2& center, float radius1, float radius2, const Camera& camera, const SDL_Color& color) {
	if (radius1 > radius2) { common::swap(radius1, radius2); }
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	int x = int(center.x) - camera.getX(), y = int(center.y) - camera.getY(), r1 = int(radius1), r12 = r1 * r1, r2 = int(radius2), r22 = r2 * r2;
	for (int i = -r2; i <= r2; ++i) {
		if (-r1 <= i && i <= r1) {
			int t1 = sqrt(r12 - i * i);
			int t2 = sqrt(r22 - i * i);
			SDL_RenderDrawLine(renderer, x - t2, y + i, x - t1, y + i);
			SDL_RenderDrawLine(renderer, x + t1, y + i, x + t2, y + i);
		}
		else {
			int t = sqrt(r22 - i * i);
			SDL_RenderDrawLine(renderer, x - t, y + i, x + t, y + i);
		}
	}
}

void drawTrigger(SDL_Renderer* renderer, const Trigger& trigger, const Camera& camera) {
	try {
		if (trigger.isActive()) {
			SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, ResourceManager::get()->getImage(Config.TriggerRingTextureKey));
			drawTexture(renderer, texture, trigger.getPosition(), camera, Relative, trigger.getOrientation(), false);
			SDL_DestroyTexture(texture);

			TriggerType triggerType = trigger.getTriggerType();
			String triggerKey = triggerType == TriggerType::HEALTH ? Config.MedPackTextureKey
				: triggerType == TriggerType::ARMOR ? Config.ArmorPackTextureKey : Config.AmmoPackTextureKey;

			texture = SDL_CreateTextureFromSurface(renderer, ResourceManager::get()->getImage(triggerKey));
			drawTexture(renderer, texture, trigger.getPosition(), camera, Relative, 0, false);
			SDL_DestroyTexture(texture);

			Vector2 pos = trigger.getPosition();
			drawString(renderer, trigger.getName().c_str(), pos.x, pos.y + Config.ActorNamePosition, camera, Relative, true, colors::white);
		}
	}
	catch (...) {}
}

SDL_Color blendColors(const SDL_Color& c1, const SDL_Color& c2, float t) {
	Uint8 r = t * c1.r + (1 - t) * c2.r;
	Uint8 g = t * c1.g + (1 - t) * c2.g;
	Uint8 b = t * c1.b + (1 - t) * c2.b;
	Uint8 a = t * c1.a + (1 - t) * c2.a;
	return { r, g, b, a };
}

void drawActor(SDL_Renderer* renderer, const Actor& actor, const Camera& camera, bool isSelected, bool showHpBar) {
	try {
		if (!actor.isDead()) {
			SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, ResourceManager::get()->getImage(Config.ActorRingTextureKey));
			Vector2 actorPos = actor.getPosition();
			fillCircle(renderer, actorPos, Config.ActorRadius, camera, actor.getTeam()->getColor());

			if (isSelected) {
				fillRing(renderer, actorPos, Config.ActorSelectionRing, Config.ActorSelectionRing + 1, camera, colors::green);
			}

			drawTexture(renderer, texture, actorPos, camera, Relative, actor.getOrientation(), false);
			SDL_DestroyTexture(texture);

			if (showHpBar) {
				SDL_Rect rect = {
					actorPos.x - Config.HealthBarWidth / 2,
					actorPos.y + Config.ActorRadius + Config.HealthBarPosition,
					Config.HealthBarWidth,
					Config.HealthBarHeight
				};
				SDL_SetRenderDrawColor(renderer,
					Config.HealthBarBackColor.r,
					Config.HealthBarBackColor.g,
					Config.HealthBarBackColor.b, 255
				);
				camera.adjustRenderArea(rect);
				SDL_RenderFillRect(renderer, &rect);

				--rect.x; --rect.y;
				int width = rect.w;
				rect.w += 2; rect.h += 2;
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
				SDL_RenderDrawRect(renderer, &rect);

				drawString(renderer, actor.getName().c_str(), actorPos.x, actorPos.y + Config.ActorNamePosition, camera, Relative, true, 
					/*actor.isWaiting() ? colors::red : actor.isMoving() ? colors::cyan : */colors::white);

				++rect.x; ++rect.y;
				rect.w = width; rect.h -= 2;
				float perc = common::clamp((float)actor.getHealth() / Config.ActorMaxHealth, 0, 1);
				SDL_Color color = perc > 0.5f ? blendColors(
					Config.HealthBarFullColor,
					Config.HealthBarHalfColor,
					(perc - 0.5f) / 0.5f)
					: blendColors(
						Config.HealthBarHalfColor,
						Config.HealthBarEmptyColor,
						perc / 0.5f);
				SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
				rect.w *= perc;
				SDL_RenderFillRect(renderer, &rect);

				perc = common::clamp((float)actor.getArmor() / Config.MaxArmor, 0, 1);
				color = Config.ArmorBarColor;
				SDL_SetRenderDrawColor(renderer, 96, 144, 208, 255);
				rect.w = width * perc;
				SDL_RenderFillRect(renderer, &rect);
			}
		}
	}
	catch (...) {}
}