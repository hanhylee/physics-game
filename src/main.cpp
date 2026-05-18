#include "raylib.h"
#include "box2d/box2d.h"
#include "level.h"

int main()
{
	int width = 1920, height = 1080;
	InitWindow(width, height, "Box2D-Raylib C++");
	SetWindowPosition(50, 50);
	SetTargetFPS(60);

	b2SetLengthUnitsPerMeter(LENGTH_UNITS_PER_METER);

	b2WorldDef worldDef = b2DefaultWorldDef();
	worldDef.gravity.y = 9.8f * LENGTH_UNITS_PER_METER;
	b2WorldId worldId = b2CreateWorld(&worldDef);

	// Scoping the level ensures its destructor is called before CloseWindow()
	{
		Level currentLevel(worldId, width, height);
		bool pause = false;

		while (!WindowShouldClose())
		{
			if (IsKeyPressed(KEY_P)) pause = !pause;

			if (!pause)
			{
				float deltaTime = GetFrameTime();
				currentLevel.Update(deltaTime);
				b2World_Step(worldId, deltaTime, 4);
			}

			BeginDrawing();
			ClearBackground(DARKGRAY);

			const char* message = "Hello Box2D C++!";
			int fontSize = 36;
			int textWidth = MeasureText(message, fontSize);
			DrawText(message, (width - textWidth) / 2, 50, fontSize, LIGHTGRAY);

			currentLevel.Draw();

			EndDrawing();
		}
	}

	// Safely close the window now that assets are unloaded
	CloseWindow();

	return 0;
}