#include <stdio.h>

#include <raylib.h>
#include <rlgl.h>
#include <imgui.h>
#include <rlImGui.h>
#include <rlFPCamera.h>

#include "pie.h"
#include "textures.h"
#include "maplibshiz.h"
#include "world.h"

int main() {
	std::unique_ptr<WzMap::MapPackage> wzmap = loadMapPackage("./data/2c-Tiny_VautEdition.wz");
	if(wzmap == nullptr) {
		TraceLog(LOG_FATAL, "failed to load map");
	}

	InitWindow(800, 450, "raylib [core] example - basic window");
	ShowCursor();
	SetTargetFPS(85);
	SetWindowMonitor(0);
	rlImGuiSetup(true);

	rlFPCamera cam;
	rlFPCameraInit(&cam, 75, (Vector3) { 0, 5, 0 });
	cam.MoveSpeed.x = 50;
	cam.MoveSpeed.y = 50;
	cam.MoveSpeed.z = 50;
	cam.MouseSensitivity = 100;
	cam.FarPlane = 5000;

	World w = World();

	int genid = w.AddObject("./data/blbrbgen.pie");
	TraceLog(LOG_INFO, "Loaded object, id: %d", genid);

	while(!WindowShouldClose()) {
		cam.UseMouse = IsMouseButtonDown(MOUSE_BUTTON_RIGHT);
		rlFPCameraUpdate(&cam);
		BeginDrawing();
			ClearBackground(DARKGRAY);
			rlFPCameraBeginMode3D(&cam);
				DrawGrid(20, 10.0f);
				w.Draw();
			rlFPCameraEndMode3D();
			DrawFPS(10, 25);
			rlImGuiBegin();
				ImGui::BeginMainMenuBar();
				ImGui::EndMainMenuBar();
			rlImGuiEnd();
		EndDrawing();
	}

	rlImGuiShutdown();

	UnloadTextures();

	w.Unload();

	CloseWindow();

	FreeModels();

	return 0;
}
