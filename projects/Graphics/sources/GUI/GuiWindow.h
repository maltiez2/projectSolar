#pragma once

#include <string>

namespace projectSolar::Graphics
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

	class DemoWindow : public GuiWindow
	{
	public:
		DemoWindow() = default;
		~DemoWindow() override = default;

		void show() override;
	};
}
