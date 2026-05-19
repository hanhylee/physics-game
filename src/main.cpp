#include "raylib.h"
#include "box2d/box2d.h"
#include "level.h"
#include <future>
// #include <chrono>

void* EnqueueTask(b2TaskCallback* task, void* taskContext, void* userContext) {
	auto* future = new std::future<void>(std::async(std::launch::async, [=]() {
		task(taskContext);
	}));
	return future;
}

void FinishTask(void* userTask, void* userContext) {
	auto* future = static_cast<std::future<void>*>(userTask);
	if (future->valid()) {
		future->wait();
	}
	delete future;
}

int main()
{
	int width = 1920, height = 1080;
	InitWindow(width, height, "Box2D-Raylib C++");
	SetWindowPosition(50, 50);
	SetTargetFPS(60);
	DisableCursor();

	b2SetLengthUnitsPerMeter(LENGTH_UNITS_PER_METER);

	b2WorldDef worldDef = b2DefaultWorldDef();
	worldDef.gravity.y = 9.8f * LENGTH_UNITS_PER_METER;

	// --- Multithreading Configuration ---
	// Automatically use the number of logical cores on the player's CPU
	worldDef.workerCount = std::thread::hardware_concurrency();
	worldDef.enqueueTask = EnqueueTask;
	worldDef.finishTask = FinishTask;
	// -----------------------------------------

	b2WorldId worldId = b2CreateWorld(&worldDef);

	// Scoping the level ensures its destructor is called before CloseWindow()
	{
		Level currentLevel(worldId, width, height);
		bool pause = false;

		while (!WindowShouldClose())
		{
			// auto frameStart = std::chrono::high_resolution_clock::now();
			if (IsKeyPressed(KEY_P)) pause = !pause;

			if (!pause)
			{
				float deltaTime = GetFrameTime();
				// auto logicStart = std::chrono::high_resolution_clock::now();
				currentLevel.Update(deltaTime);
				// auto logicEnd = std::chrono::high_resolution_clock::now();
				// auto physicsStepStart = std::chrono::high_resolution_clock::now();
				b2World_Step(worldId, deltaTime, 4);
				// auto physicsStepEnd = std::chrono::high_resolution_clock::now();
			}

			// auto renderStart = std::chrono::high_resolution_clock::now();
			BeginDrawing();
			ClearBackground(DARKGRAY);

			const char* message = "Hello Box2D C++!";
			int fontSize = 36;
			int textWidth = MeasureText(message, fontSize);
			DrawText(message, (width - textWidth) / 2, 50, fontSize, LIGHTGRAY);

			currentLevel.Draw();

			EndDrawing();
			// auto renderEnd = std::chrono::high_resolution_clock::now();
			// auto frameEnd = std::chrono::high_resolution_clock::now();
			// std::chrono::duration<double, std::milli> totalFrame = frameEnd - frameStart;
			// std::chrono::duration<double, std::milli> logicTime = logicEnd - logicStart;
			// std::chrono::duration<double, std::milli> physTime = physicsStepEnd - physicsStepStart;
			// std::chrono::duration<double, std::milli> renderTime = renderEnd - renderStart;
			//
			// static int frameCount = 0;
			// if (frameCount++ % 60 == 0) {
			// 	printf("[REAL METRICS] Enemies: %d | Frame Time: %0.2fms | Logic: %0.2fms | Box2D Step: %0.2fms | Render Pipeline: %0.2fms\n",
			// 		   currentLevel.GetEnemyCount(), totalFrame.count(), logicTime.count(), physTime.count(), renderTime.count());
			// }
		}
	}

	// Safely close the window now that assets are unloaded
	CloseWindow();

	return 0;
}