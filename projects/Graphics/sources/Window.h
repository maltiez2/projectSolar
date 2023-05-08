#pragma once

#include "Input/Input.h"

#include "../vendor/opengl/imgui_impl_glfw.h"

#include <string>
#include <functional>


namespace projectSolar::Graphics
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
		InputEventsManager* eventsManager = nullptr;

		GuiProperties guiProperties;
	};

	// @TODO Should be one window at a time currently, should add multi window support or make it singleton
	class Window
	{
	public:
		explicit Window(const WindowProperties& properties = WindowProperties());
		~Window();

		void startFrame();

		// Getters
		GLFWwindow* getNativeWindow();
		uint32_t    getWidth() const;
		uint32_t    getHeight() const;
		bool        isVSync() const;
		InputEventsManager& getEventsManager();
		InputManager& getInputManager();

		// Setters
		void setVSync(bool enabled);
		void setFont(const std::string& font);
		void setSize(uint32_t width, uint32_t height);

		InputEventsManager m_eventsManager;
		InputManager m_inputManager;
		WindowProperties m_properties;

	private:
		const char* fontsFolder = "resources/fonts";
		const char* glslVersion = "#version 130";
		const int glMajorVersion = 3;
		const int glMinorVersion = 3;
		
		GLFWwindow* m_window;

		void init(const WindowProperties& properties);
		void shutdown();

		void setupImGui(const GuiProperties& properties);
		GLFWmonitor* setUpFullscreen();
	};
}


