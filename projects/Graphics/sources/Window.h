#pragma once

#include "Events/Event.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "Events/ApplicationEvent.h"
#include "Input/Input.h"

#include "../vendor/opengl/imgui_impl_glfw.h"

#include <string>
#include <functional>


namespace projectSolar
{
	struct GuiProperties
	{
		std::string font = "default.ttf";
	};
	
	struct WindowProperties
	{
		std::string title   = "Project 'Solar'";
		uint32_t width      = 1600;
		uint32_t height     = 900;
		 int32_t monitor    = -1;
		bool VSync          = true;
		bool fullScreen     = true;

		GuiProperties guiProperties;
	};

	// @TODO Should be one window at a time currently, should add multi window support or make it singleton
	class Window
	{
	public:
		using eventCallbackFunction = std::function<void(Event&)>;
		
		struct WindowData
		{
			std::string title;
			uint32_t    width;
			uint32_t    height;
			int32_t    monitor;
			bool VSync;
			bool fullScreen;

			InputEventsManager* eventsManager;

			eventCallbackFunction eventCallback;
		};
		
		explicit Window(const WindowProperties& properties = WindowProperties());
		~Window();

		void startFrame();

		// Getters
		GLFWwindow* getNativeWindow();
		uint32_t    getWidth() const;
		uint32_t    getHeight() const;
		bool        isVSync() const;

		// Setters
		void setVSync(bool enabled);
		void setFont(const std::string& font);
		void setSize(uint32_t width, uint32_t height);
		void setEventCallback(const eventCallbackFunction& callback);

	private:
		const char* fontsFolder = "resources/fonts";
		const char* glslVersion = "#version 130";
		const int glMajorVersion = 3;
		const int glMinorVersion = 3;

		InputEventsManager eventsManager;
		
		GLFWwindow* m_window;
		WindowData m_properties;

		void init(const WindowProperties& properties);
		void shutdown();

		void setupImGui(const GuiProperties& properties);
		void setupEvents();
		GLFWmonitor* setUpFullscreen();
	};
}


