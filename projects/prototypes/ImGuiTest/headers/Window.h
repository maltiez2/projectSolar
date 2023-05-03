#pragma once

#include <string>

namespace imGuiTest
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

		/*WindowProperties(const std::string& title = "Project 'Solar'",
			uint32_t width = 1600,
			uint32_t height = 900,
			bool VSync = false)
			: title(title), width(width), height(height), VSync(VSync)
		{
		}*/
	};

	class Window
	{
	public:
		explicit Window(const WindowProperties& properties = WindowProperties());
		~Window();

		// Getters
		uint32_t getWidth() const;
		uint32_t getHeight() const;
		void*    getNativeWindow();
		bool     isVSync() const;

		// Setters
		void setVSync(bool enabled);
		void setFont(const std::string& font);

	private:
		const char* fontsFolder = "resources/fonts";
		const char* glslVersion = "#version 130";
		
		void* m_window;
		WindowProperties m_properties;

		void init(const WindowProperties& properties);
		void shutdown();

		void setupImGui(const GuiProperties& properties);
	};
}


