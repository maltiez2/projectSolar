#pragma once

#include <string>
#include <Eigen/Eigen>

namespace projectSolar
{
	class GuiWindow
	{
	public:
		bool showFlag = true;

		GuiWindow() = default;
		virtual ~GuiWindow() = default;
		
		virtual void show() = 0;
	};

	class NotificationWindow : public GuiWindow
	{
	public:
		NotificationWindow(const std::string& title, const std::string& message);
		~NotificationWindow() override = default;

		void show() override;

	private:
		std::string m_title;
		std::string m_message;
	};
	class DebugWindow : public GuiWindow
	{
	public:
		bool showDemoWindow = false;
		
		DebugWindow() = default;
		~DebugWindow() override = default;

		void show() override;

		float scale = 1.0f;
		float timeScale = 1.0f;
		bool runSimulation = false;
		bool closeApp = false;
	};
	class DemoWindow : public GuiWindow
	{
	public:
		DemoWindow() = default;
		~DemoWindow() override = default;

		void show() override;
	};
	class PropulsionControlWindow : public GuiWindow
	{
	public:
		PropulsionControlWindow() = default;
		~PropulsionControlWindow() override = default;

		void show() override;

		float direction = 0.0;
		float magnitude = 0.0;
		bool followPlayer = true;
	};
}
