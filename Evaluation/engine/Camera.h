struct SDL_Renderer;
struct SDL_Texture;
struct SDL_Color;
struct SDL_Rect;

class Vector2;
class Aabb;
class Segment;
class Trigger;
class Actor;

enum PositionType {
	Absolute,
	Relative
};

class Camera {
public:
	Camera(int x, int y, int xMin, int xMax, int yMin, int yMax);

	void update(bool up, bool right, bool down, bool left);

	void adjustRenderArea(SDL_Rect& rect) const;
	SDL_Rect getRenderArea(int x, int y, int width, int height) const;
	SDL_Rect getRenderArea(float x, float y, float width, float height) const;

	int getX() const;
	int getY() const;

private:
	int _x;
	int _y;
	int _xMin;
	int _xMax;
	int _yMin;
	int _yMax;
	int _speed;
};

SDL_Color blendColors(const SDL_Color& c1, const SDL_Color& c2, float t);

void drawTexture(SDL_Renderer* renderer, SDL_Texture* texture, const Vector2& position, 
	const Camera& camera, PositionType positionType);
void drawTexture(SDL_Renderer* renderer, SDL_Texture* texture, const Vector2& center, 
	const Camera& camera, PositionType positionType, float orientation, bool horizontalFlip);
void drawString(SDL_Renderer* renderer, const char* string, int x, int y, const Camera& camera, 
	PositionType positionType, bool centered, const SDL_Color& color);

void drawAabb(SDL_Renderer* renderer, const Aabb& aabb, const Camera& camera, const SDL_Color& color);
void fillAabb(SDL_Renderer* renderer, const Aabb& aabb, const Camera& camera, const SDL_Color& color);
void drawSegment(SDL_Renderer* renderer, const Segment& segment, const Camera& camera, const SDL_Color& color);
void drawPoint(SDL_Renderer* renderer, const Vector2& point, const Camera& camera, const SDL_Color& color);
void drawCircle(SDL_Renderer* renderer, const Vector2& center, float radius, const Camera& camera, const SDL_Color& color);
void fillCircle(SDL_Renderer* renderer, const Vector2& center, float radius, const Camera& camera, const SDL_Color& color);
void fillRing(SDL_Renderer* renderer, const Vector2& center, float radius1, float radius2, const Camera& camera, const SDL_Color& color);

void drawTrigger(SDL_Renderer* renderer, const Trigger& trigger, const Camera& camera);
void drawActor(SDL_Renderer* renderer, const Actor& actor, const Camera& camera, bool isSelected, bool showHpBar);
