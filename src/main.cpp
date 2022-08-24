#include <stdio.h>

#include <raylib.h>
#include <rlgl.h>
#include <imgui.h>
#include <imgui_stdlib.h>
#include <rlImGui.h>
#include <rlFPCamera.h>
#include <physfs.h>

#include "pie.h"
#include "textures.h"
#include "maplibshiz.h"
#include "world.h"
#include "config.h"
#include "misc.h"

int main(int argc, char** argv) {
	// std::unique_ptr<WzMap::MapPackage> wzmap = loadMapPackage("./data/2c-Tiny_VautEdition.wz");
	// if(wzmap == nullptr) {
	// 	TraceLog(LOG_FATAL, "failed to load map");
	// }

	if(!PHYSFS_init(argv[0])) {
		TraceLog(LOG_FATAL, "Failed to init physfs");
	}

	if(!LoadConfig()) {
		TraceLog(LOG_WARNING, "Failed to load config!");
	}

	for(std::string s : conf.mountpaths) {
		std::string fullpath = conf.wzdata + s;
		if(!PHYSFS_mount(fullpath.c_str(), NULL, 0)) {
			TraceLog(LOG_ERROR, "Failed to mount %s", fullpath.c_str());
		}
	}

	SetTraceLogCallback(loggingCallback);

	InitWindow(800, 450, "Cobra map editor");
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

	static bool wPreferences = false;

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
			if(ImGui::BeginMainMenuBar()) {
				if(ImGui::BeginMenu("Edit")) {
					ImGui::MenuItem("Preferences", NULL, &wPreferences);
					ImGui::EndMenu();
				}
				ImGui::EndMainMenuBar();
			}
			if(wPreferences) {
				ImGui::Begin("Preferences", &wPreferences);
				ImGui::InputText("WZ data path", &conf.wzdata);
				ImGui::Text("Loading paths (ordered)");
				if(ImGui::Button("Add")) {
					conf.mountpaths.push_back("");
				}
				for(int i = 0; i < conf.mountpaths.size(); i++) {
					ImGui::PushID(i);
					ImGui::InputText("", &conf.mountpaths[i]);
					ImGui::PopID();
				}
				if(ImGui::Button("Save")) {
					SaveConfig();
				}
				ImGui::End();
			}
			rlImGuiEnd();
		EndDrawing();
	}

	rlImGuiShutdown();

	UnloadTextures();

	w.Unload();

	CloseWindow();

	FreeModels();

	if(!PHYSFS_deinit()) {
		TraceLog(LOG_ERROR, "Failed to deinit physfs");
	}

	SaveConfig();

	return 0;
}
