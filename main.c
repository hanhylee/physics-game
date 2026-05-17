#include "raylib.h"
#include "raymath.h"
#include "box2d/box2d.h"

#include <assert.h>

// This shows how to use Box2D v3 with raylib.
// It also show how to use Box2D with pixel units.

typedef struct Entity
{
	b2BodyId bodyId;
	b2Vec2 extent;
	Texture texture;
	float scale;
} Entity;

static Vector2 ToVector2(b2Vec2 v) { return (Vector2){ v.x, v.y }; }
static b2Vec2 ToB2Vec2(Vector2 v) { return (b2Vec2){ v.x, v.y }; }

void DrawEntity(const Entity* entity)
{
	// The boxes were created centered on the bodies, but raylib draws textures starting at the top left corner.
	// b2Body_GetWorldPoint gets the top left corner of the box accounting for rotation.
	b2Vec2 p = b2Body_GetWorldPoint(entity->bodyId, (b2Vec2) { -entity->extent.x, -entity->extent.y });
	b2Rot rotation = b2Body_GetRotation(entity->bodyId);
	float radians = b2Rot_GetAngle(rotation);

	Vector2 ps = {p.x, p.y};
	DrawTextureEx(entity->texture, ps, RAD2DEG * radians, entity->scale, WHITE);

	// I used these circles to ensure the coordinates are correct
	//DrawCircleV(ps, 5.0f, BLACK);
	//p = b2Body_GetWorldPoint(entity->bodyId, (b2Vec2){0.0f, 0.0f});
	//ps = (Vector2){ p.x, p.y };
	//DrawCircleV(ps, 5.0f, BLUE);
	//p = b2Body_GetWorldPoint(entity->bodyId, (b2Vec2){ entity->extent.x, entity->extent.y });
	//ps = (Vector2){ p.x, p.y };
	//DrawCircleV(ps, 5.0f, RED);
}

#define GROUND_COUNT 14
#define BOX_COUNT 10
#define PLAYER_COUNT 1
// #define float lengthUnitsPerMeter = 128.0f;
#define LENGTH_UNITS_PER_METER 128.0f;

Entity AttachWeapon(b2WorldId worldId, const Entity* player, Texture texture, b2Vec2 extent, b2Polygon* polygon)
{
	Entity weapon = { 0 };
	weapon.texture = texture;
	weapon.extent = extent;
	weapon.scale = 4.0f;

	b2Vec2 playerPos = b2Body_GetPosition(player->bodyId);

	b2BodyDef bodyDef = b2DefaultBodyDef();
	bodyDef.type = b2_dynamicBody;
	bodyDef.gravityScale = 3.0f;

	bodyDef.position = (b2Vec2){ playerPos.x + player->extent.x + extent.x, playerPos.y };
	weapon.bodyId = b2CreateBody(worldId, &bodyDef);


	b2ShapeDef shapeDef = b2DefaultShapeDef();
	b2CreatePolygonShape(weapon.bodyId, &shapeDef, polygon);

	b2RevoluteJointDef jointDef = b2DefaultRevoluteJointDef();

	jointDef.base.bodyIdA = player->bodyId;
	jointDef.base.bodyIdB = weapon.bodyId;
	jointDef.base.localFrameA.p = (b2Vec2){ player->extent.x / 2 , player->extent.y / 2 };
	jointDef.base.localFrameB.p = (b2Vec2){ 0.0f, extent.y * 10 };
	b2CreateRevoluteJoint(worldId, &jointDef);

	return weapon;
}

void FollowCursor(const Entity* player) {
	float lengthUnitsPerMeter = LENGTH_UNITS_PER_METER;
	Vector2 mousePos = GetMousePosition();
	b2Vec2 cursorPos = { mousePos.x, mousePos.y };
	// Entity player = playerEntities[0];
	b2BodyId playerId = player->bodyId;
	b2Vec2 playerPos = b2Body_GetPosition(playerId);
	b2Vec2 cursorPullForce = {
		cursorPos.x - playerPos.x,
		cursorPos.y - playerPos.y
	};
	float mass = b2Body_GetMass(playerId);
	float strength = mass * lengthUnitsPerMeter * 2.0f;
	b2Vec2 dir = b2Normalize(cursorPullForce);
	b2Vec2 force = { dir.x * strength, dir.y * strength };
	b2Body_ApplyLinearImpulseToCenter(playerId, force, true);
}

int main(void)
{
	int width = 1920, height = 1080;
	InitWindow(width, height, "box2d-raylib");

	SetTargetFPS(60);

	// 128 pixels per meter is a appropriate for this scene. The boxes are 128 pixels wide.
	float lengthUnitsPerMeter = LENGTH_UNITS_PER_METER;
	b2SetLengthUnitsPerMeter(lengthUnitsPerMeter);

	b2WorldDef worldDef = b2DefaultWorldDef();

	// Realistic gravity is achieved by multiplying gravity by the length unit.
	worldDef.gravity.y = 9.8f * lengthUnitsPerMeter;
	b2WorldId worldId = b2CreateWorld(&worldDef);

	Texture groundTexture = LoadTexture("assets/ground.png");
	Texture boxTexture = LoadTexture("assets/box.png");
	Texture playerTexture = LoadTexture("assets/AmberBall-200.png");
	Texture swordTexture = LoadTexture("assets/sword1.png");

	b2Vec2 groundExtent = { 0.5f * groundTexture.width, 0.5f * groundTexture.height };
	b2Vec2 boxExtent = { 0.5f * boxTexture.width, 0.5f * boxTexture.height };
	b2Vec2 playerExtent = { 0.25f * playerTexture.width, 0.25f * playerTexture.height };
	b2Vec2 swordExtent = { 0.5f * swordTexture.width, 0.5f * swordTexture.height };

	// These polygons are centered on the origin and when they are added to a body they
	// will be centered on the body position.
	b2Polygon groundPolygon = b2MakeBox(groundExtent.x, groundExtent.y);
	b2Polygon boxPolygon = b2MakeBox(boxExtent.x, boxExtent.y);
	b2Polygon playerPolygon = b2MakeBox(playerExtent.x, playerExtent.y);
	b2Polygon swordPolygon = b2MakeBox(swordExtent.x, swordExtent.y);

	Entity groundEntities[GROUND_COUNT] = { 0 };
	for (int i = 0; i < GROUND_COUNT; ++i)
	{
		Entity* entity = groundEntities + i;
		b2BodyDef bodyDef = b2DefaultBodyDef();
		bodyDef.position = (b2Vec2){ (2.0f * i + 2.0f) * groundExtent.x, height - groundExtent.y - 100.0f};

		// I used this rotation to test the world to screen transformation
		//bodyDef.rotation = b2MakeRot(0.25f * b2_pi * i);

		entity->bodyId = b2CreateBody(worldId, &bodyDef);
		entity->extent = groundExtent;
		entity->texture = groundTexture;
		entity->scale = 1.0f;
		b2ShapeDef shapeDef = b2DefaultShapeDef();
		b2CreatePolygonShape(entity->bodyId, &shapeDef, &groundPolygon);
	}

	Entity boxEntities[BOX_COUNT] = { 0 };
	int boxIndex = 0;
	for (int i = 0; i < 4; ++i)
	{
		float y = height - groundExtent.y - 100.0f - (2.5f * i + 2.0f) * boxExtent.y - 20.0f;

		for (int j = i; j < 4; ++j)
		{
			float x = 0.5f * width + (3.0f * j - i - 3.0f) * boxExtent.x;
			assert(boxIndex < BOX_COUNT);

			Entity* entity = boxEntities + boxIndex;
			b2BodyDef bodyDef = b2DefaultBodyDef();
			bodyDef.type = b2_dynamicBody;
			bodyDef.position = (b2Vec2){ x, y };
			entity->bodyId = b2CreateBody(worldId, &bodyDef);
			entity->texture = boxTexture;
			entity->extent = boxExtent;
			entity->scale = 1.0f;
			b2ShapeDef shapeDef = b2DefaultShapeDef();
			b2CreatePolygonShape(entity->bodyId, &shapeDef, &boxPolygon);

			boxIndex += 1;
		}
	}

	Entity playerEntities[PLAYER_COUNT] = { 0 };
	int playerIndex = 0;
	for (int i = 0; i < 1; ++i)
	{
		float y = height - groundExtent.y - 100.0f - (2.5f * i + 2.0f) * playerExtent.y - 20.0f;

		for (int j = i; j < 1; ++j)
		{
			float x = 0.5f * width + (3.0f * j - i - 3.0f) * playerExtent.x;
			assert(playerIndex < PLAYER_COUNT);

			Entity* entity = playerEntities + playerIndex;
			b2BodyDef bodyDef = b2DefaultBodyDef();
			bodyDef.type = b2_dynamicBody;
			bodyDef.angularDamping = 10.8f;
			bodyDef.position = (b2Vec2){ x, y };
			entity->bodyId = b2CreateBody(worldId, &bodyDef);
			entity->texture = playerTexture;
			entity->extent = playerExtent;
			entity->scale = 0.5f;
			b2ShapeDef shapeDef = b2DefaultShapeDef();
			b2CreatePolygonShape(entity->bodyId, &shapeDef, &playerPolygon);

			playerIndex += 1;
		}
	}

	Entity weaponEntity = AttachWeapon(worldId, playerEntities, swordTexture, swordExtent, &swordPolygon);

	bool pause = false;

	while (!WindowShouldClose())
	{
		if (IsKeyPressed(KEY_P))
		{
			pause = !pause;
		}

		if (pause == false)
		{
			float deltaTime = GetFrameTime();
			Entity player = playerEntities[0];
			FollowCursor(&player);
			b2World_Step(worldId, deltaTime, 4);
		}

		BeginDrawing();
		ClearBackground(DARKGRAY);

		const char* message = "Hello Box2D!";
		int fontSize = 36;
		int textWidth = MeasureText("Hello Box2D!", fontSize);
		DrawText(message, (width - textWidth) / 2, 50, fontSize, LIGHTGRAY);

		for (int i = 0; i < GROUND_COUNT; ++i)
		{
			DrawEntity(groundEntities + i);
		}

		for (int i = 0; i < BOX_COUNT; ++i)
		{
			DrawEntity(boxEntities + i);
		}

		for (int i = 0; i < PLAYER_COUNT; ++i)
		{
			DrawEntity(playerEntities + i);
		}

		DrawEntity(&weaponEntity);

		EndDrawing();
	}

	UnloadTexture(groundTexture);
	UnloadTexture(boxTexture);
	UnloadTexture(playerTexture);
	UnloadTexture(swordTexture);

	CloseWindow();

	return 0;
}
