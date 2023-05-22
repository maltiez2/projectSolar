#pragma once

#include "Logger.h"

#include <map>
#include <string>


namespace projectSolar::Graphics
{
	class GuiWindow;
	class Window;
	
	class GuiWindowsManager
	{
	public:
		GuiWindowsManager() = default;
		~GuiWindowsManager();
		
		template<typename WindowType, typename ... Args>
		bool add(const size_t& id, bool show, const Args& ... args)
		{
			if (m_windows.contains(id))
			{
				LOG_ERROR("[GuiWindowsManager][add()] Window with id '", id, "' already exists");
				return false;
			}

			WindowType* window = new WindowType(args...);

			m_windows.try_emplace(id, window);
			window->showFlag = show;

			return true;
		}

		template<typename WindowType>
		WindowType* get(const size_t& id)
		{
			if (!m_windows.contains(id))
			{
				LOG_ERROR("[GuiWindowsManager][get()] Window with id '", id, "' does not exist");
				return nullptr;
			}

			return (WindowType*)m_windows.at(id);
		}

		bool del(const size_t& id);
		bool show(const size_t& id, bool show);
		bool exist(const size_t& id);
		bool shown(const size_t& id);

		const std::map<size_t, GuiWindow*>& getWindows();

	private:
		std::map<size_t, GuiWindow*> m_windows;
	};
	
	class GuiRenderer
	{
	public:
		explicit GuiRenderer(Window& window, GuiWindowsManager& windowsManager);
		~GuiRenderer() = default;

		void render();
		void begin();
		void end();

		inline GuiWindowsManager& getWindowsManager() { return m_windowsManager; }

	private:
		Window& m_window;
		GuiWindowsManager& m_windowsManager;
	};
}