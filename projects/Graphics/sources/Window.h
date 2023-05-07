#pragma once

#include "../vendor/opengl/imgui_impl_glfw.h"

#include <string>


namespace projectSolar
{
	struct GuiProperties
	{
		std::string font = "default.ttf";
	};
	
	struct WindowProperties
	{
		std::string title  = "Project 'Solar'";
		uint32_t    width  = 1600;
		uint32_t    height = 900;
		bool        VSync  = true;

		GuiProperties guiProperties;
	};

	// @TODO Should be one window at a time currently, should add multi window support or make it singleton
	class Window
	{
	public:
		explicit Window(const int32_t& monitor = -1, const WindowProperties& properties = WindowProperties());
		~Window();

		bool startFrame();

		// Getters
		GLFWwindow* getNativeWindow();
		uint32_t    getWidth() const;
		uint32_t    getHeight() const;
		bool        isVSync() const;

		// Setters
		void setVSync(bool enabled);
		void setFont(const std::string& font);
		void setSize(uint32_t width, uint32_t height);

	private:
		struct WindowData
		{
			std::string title;
			uint32_t    width;
			uint32_t    height;
			bool        VSync;
		};
		
		const char* fontsFolder = "resources/fonts";
		const char* glslVersion = "#version 130";
		const int glMajorVersion = 3;
		const int glMinorVersion = 3;
		
		GLFWwindow* m_window;
		WindowProperties m_properties;
		int32_t m_monitor;

		void init(const WindowProperties& properties);
		void shutdown();

		void setupImGui(const GuiProperties& properties);
	};
}


