#pragma once

#include "Window.h"
#include "GuiWindow.h"
#include "Logger.h"

#include <string>
#include <unordered_map>

namespace projectSolar
{
	class GuiWindowsManager
	{
	public:
		GuiWindowsManager() = default;
		~GuiWindowsManager();
		
		template<typename WindowType, typename ... Args>
		bool add(const std::string& id, bool show, const Args& ... args)
		{
			if (m_windows.contains(id))
			{
				LOG_ERROR("[GuiWindowsManager][add()] Window with id '" + id + "' already exists");
				return false;
			}

			WindowType* window = new WindowType(args...);

			m_windows.try_emplace(id, window);
			window->showFlag = show;

			return true;
		}

		template<typename WindowType>
		WindowType* get(const std::string& id)
		{
			if (!m_windows.contains(id))
			{
				LOG_ERROR("[GuiWindowsManager][get()] Window with id '" + id + "' does not exist");
				return nullptr;
			}

			return (WindowType*)m_windows.at(id);
		}

		bool del(const std::string& id);
		bool show(const std::string& id, bool show);
		bool exist(const std::string& id);
		bool shown(const std::string& id);

		const std::unordered_map<std::string, GuiWindow*>& getWindows();

	private:
		std::unordered_map<std::string, GuiWindow*> m_windows;
	};
	
	class GuiRenderer
	{
	public:
		explicit GuiRenderer(Window& window, GuiWindowsManager& windowsManager);
		~GuiRenderer() = default;

		void render();

		inline GuiWindowsManager& getWindowsManager() { return m_windowsManager; }

	private:
		Window& m_window;
		GuiWindowsManager& m_windowsManager;
	};
}