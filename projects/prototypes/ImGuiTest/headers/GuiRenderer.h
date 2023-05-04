#pragma once

#include "Window.h"
#include "GuiWindow.h"

#include <string>
#include <unordered_map>

namespace imGuiTest
{
	class GuiRenderer
	{
	public:
		GuiRenderer(Window* window);
		~GuiRenderer();

		void render();

		bool addGuiWindow(const std::string& id, GuiWindow* window, bool show);
		bool showGuiWindow(const std::string& id, bool show);
		bool delGuiWindow(const std::string& id);
		bool ifGuiWindowExists(const std::string& id) const;
		bool ifGuiWindowShown(const std::string& id) const;

	private:
		Window* m_window;
		std::unordered_map <std::string, GuiWindow*> m_guiWindows;
		std::unordered_map < std::string, bool> m_showWindows;
	};
}