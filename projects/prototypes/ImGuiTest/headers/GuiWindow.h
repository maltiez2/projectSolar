#pragma once

#include <string>

namespace imGuiTest
{
	class GuiWindow
	{
	public:
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

}
