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

	class Window
	{
	public:
		explicit Window(const WindowProperties& properties = WindowProperties());
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
		const char* fontsFolder = "resources/fonts";
		const char* glslVersion = "#version 130";
		const int glMajorVersion = 3;
		const int glMinorVersion = 3;
		
		GLFWwindow* m_window;
		WindowProperties m_properties;

		void init(const WindowProperties& properties);
		void shutdown();

		void setupImGui(const GuiProperties& properties);
	};
}


