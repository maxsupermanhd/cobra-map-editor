#include <stdio.h>

#include <raylib.h>
#include <rlgl.h>
#include <imgui.h>
#include <rlImGui.h>
#include <rlFPCamera.h>

#include "pie.h"
#include "textures.h"
#include "maplibshiz.h"

int main() {
	InitWindow(800, 450, "raylib [core] example - basic window");
	ShowCursor();
	SetTargetFPS(85);
	SetWindowMonitor(0);
	rlImGuiSetup(true);

	std::unique_ptr<WzMap::MapPackage> wzmap = loadMapPackage("./data/8c-Stone-Jungle-E.wz");

	rlFPCamera cam;
	rlFPCameraInit(&cam, 75, (Vector3) { 0, 5, 0 });
	cam.MoveSpeed.x = 50;
	cam.MoveSpeed.y = 50;
	cam.MoveSpeed.z = 50;
	cam.MouseSensitivity = 100;
	cam.FarPlane = 5000;

	Mesh m = { 0 };
	if(!LoadFromPIE("./data/blbrbgen.pie", &m)) {
		abort();
	}

	Material mat = LoadMaterialDefault();
	mat.maps[MATERIAL_MAP_DIFFUSE].texture = GetTexture("./data/page-7-barbarians-arizona.png");

	for(unsigned int i = 0; i < m.vertexCount; i++) {
		m.texcoords[i*2+0] /= mat.maps[MATERIAL_MAP_DIFFUSE].texture.height;
		m.texcoords[i*2+1] /= mat.maps[MATERIAL_MAP_DIFFUSE].texture.width;
	}

	Shader alphaDiscardShader = LoadShader(NULL, "data/discard_alpha.fs");
	mat.shader = alphaDiscardShader;

	Matrix i = MatrixIdentity();

	UploadMesh(&m, false);

	while(!WindowShouldClose()) {
		cam.UseMouse = IsMouseButtonDown(MOUSE_BUTTON_RIGHT);
		rlFPCameraUpdate(&cam);
		BeginDrawing();
			ClearBackground(DARKGRAY);
			rlFPCameraBeginMode3D(&cam);
				DrawGrid(20, 10.0f);
				DrawMesh(m, mat, i);
			rlFPCameraEndMode3D();
			DrawFPS(10, 25);
			rlImGuiBegin();
				ImGui::BeginMainMenuBar();
				ImGui::EndMainMenuBar();
			rlImGuiEnd();
		EndDrawing();
	}

	rlImGuiShutdown();
	UnloadShader(alphaDiscardShader);
	UnloadTextures();

	CloseWindow();

	FreeModels();

	return 0;
}
