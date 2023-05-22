#pragma once

#include "GuiApi.h"

#include <vector>
#include <string>


namespace projectSolar::Windows
{
	class PropulsionControl : public Graphics::GuiWindow
	{
	public:
		PropulsionControl() = default;
		~PropulsionControl() override = default;

		void show() override;

		float direction = 0.0;
		float magnitude = 0.0;
	};

	class Debug : public Graphics::GuiWindow
	{
	public:
		Debug() = default;
		~Debug() override = default;

		void show() override;

		float scale = 1.0f;
		float simLoad = 0.5f;
		float simRate = 1.0f;
		float prevSimLoad = 0.0f;
		float prevSimRate = 0.0f;
		size_t stepsPerFrame = 0;
		float secondsPerStep = 0.0f;
		std::vector<std::string> objUnderCursor = {};

		bool runSimulation = false;
		bool followPlayer = false;
		bool showDemoWindow = false;
		bool generateDebugData = false;
		bool saveData = false;
		bool loadData = false;

	};
}
