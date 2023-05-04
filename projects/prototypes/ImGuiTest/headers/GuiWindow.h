#pragma once

#include <string>

namespace imGuiTest
{
	class GuiWindow
	{
	public:
		bool showFlag = true;

		GuiWindow() = default;
		virtual ~GuiWindow() = default;
		
		virtual void show();
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
	};
	class DemoWindow : public GuiWindow
	{
	public:
		DemoWindow() = default;
		~DemoWindow() override = default;

		void show() override;
	};

}
