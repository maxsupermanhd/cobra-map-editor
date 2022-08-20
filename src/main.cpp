#include <stdio.h>

#include "raylib.h"
#include "imgui.h"
#include "rlImGui.h"
#include "rlFPCamera.h"
#include "pie.h"


int main() {
	InitWindow(800, 450, "raylib [core] example - basic window");
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
	while(!WindowShouldClose()) {
		cam.UseMouse = IsMouseButtonDown(MOUSE_BUTTON_RIGHT);
		rlFPCameraUpdate(&cam);
		BeginDrawing();
			ClearBackground(RAYWHITE);
			rlFPCameraBeginMode3D(&cam);
				DrawGrid(20, 10.0f);
			rlFPCameraEndMode3D();
			DrawFPS(10, 25);
			rlImGuiBegin();
				ImGui::BeginMainMenuBar();
				ImGui::EndMainMenuBar();
			rlImGuiEnd();
		EndDrawing();
	}

	rlImGuiShutdown();

	CloseWindow();

	return 0;
}