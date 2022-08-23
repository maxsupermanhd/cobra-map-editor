#include <stdio.h>

#include "raylib.h"
#include "rlgl.h"
#include "imgui.h"
#include "rlImGui.h"
#include "rlFPCamera.h"
#include "pie.h"
#include "textures.h"

#include <wzmaplib/map.h>
#include <wzmaplib/map_package.h>
#include <ZipIOProvider.h>
#include <iostream>
#include <sstream>
#include <string>

class MapToolDebugLogger : public WzMap::LoggingProtocol
{
public:
	MapToolDebugLogger(bool verbose)
	: verbose(verbose)
	{ }
	virtual ~MapToolDebugLogger() { }
	virtual void printLog(WzMap::LoggingProtocol::LogLevel level, const char *function, int line, const char *str) override
	{
		std::ostream* pOutputStream = &(std::cout);
		if (level == WzMap::LoggingProtocol::LogLevel::Error)
		{
			pOutputStream = &(std::cerr);
		}
		std::string levelStr;
		switch (level)
		{
			case WzMap::LoggingProtocol::LogLevel::Info_Verbose:
			case WzMap::LoggingProtocol::LogLevel::Info:
				if (!verbose) { return; }
				levelStr = "INFO";
				break;
			case WzMap::LoggingProtocol::LogLevel::Warning:
				levelStr = "WARNING";
				break;
			case WzMap::LoggingProtocol::LogLevel::Error:
				levelStr = "ERROR";
				break;
		}
		(*pOutputStream) << levelStr << ": [" << function << ":" << line << "] " << str << std::endl;
	}
private:
	bool verbose = false;
};

static std::unique_ptr<MapPackage> loadMapPackage(const char* pathToWzPackage, std::shared_ptr<WzMap::LoggingProtocol> logger)
{
	auto zipArchive = WzMapZipIO::openZipArchiveFS(pathToWzPackage);
	if (!zipArchive)
	{
		std::cerr << "Failed to open map archive file: " << mapArchive << std::endl;
		return nullopt;
	}

	auto wzMapPackage = WzMap::MapPackage::loadPackage(mapPackageContentsPath, logger, mapIO);
	if (!wzMapPackage)
	{
		std::cerr << "Failed to load map archive package from: " << mapPackageContentsPath << std::endl;
		return nullopt;
	}

	return wzMapPackage;
}

int main() {
	InitWindow(800, 450, "raylib [core] example - basic window");
	ShowCursor();
	SetTargetFPS(85);
	SetWindowMonitor(0);
	rlImGuiSetup(true);

	bool verboseMapLogging = true;
	auto mapLogger = std::make_shared<MapToolDebugLogger>(verboseMapLogging);
	auto mapPackage = loadMapPackage("./data/8c-Stone-Jungle-E.wz", mapLogger);
	if (!mapPackage)
	{
		// Failed to load map package
		abort();
	}
	auto loadedMap = mapPackage->loadMap(0, logger);
	if (!loadedMap)
	{
		// Failed to load map
		abort();
	}
	auto loadedMapFormat = loadedMap->loadedMapFormat();
	if (!loadedMapFormat.has_value())
	{
		// Failure loading some or all of map data
		abort();
	}
	TraceLog(LOG_INFO, "Map format: %d", (int)loadedMapFormat.value());

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
