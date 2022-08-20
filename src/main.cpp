#include <stdio.h>

#include "raylib.h"
#include "imgui.h"
#include "rlImGui.h"

int main() {
	InitWindow(800, 450, "raylib [core] example - basic window");
	SetTargetFPS(85);
	SetWindowMonitor(0);
	rlImGuiSetup(true);
	bool open = true;

	while(!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(RAYWHITE);

		rlImGuiBegin();
		ImGui::ShowDemoWindow(&open);
		rlImGuiEnd();

		EndDrawing();
	}

	CloseWindow();

	return 0;
}