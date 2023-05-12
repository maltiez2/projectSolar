#pragma once

#include "Graphics.h"


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
		bool runSimulation = false;
		bool followPlayer = false;
		bool showDemoWindow = false;
	};
}
