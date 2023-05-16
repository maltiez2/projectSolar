#pragma once

#include "Layer.h"

#include <memory>

namespace projectSolar::Graphics
{
	class GuiWindowsManager;
	class GuiRenderer;
	class Window;
	class InputEvent;
}


namespace projectSolar::Layers
{
	class GuiLayer : public Layer
	{
	public:
		GuiLayer(std::shared_ptr<Graphics::Window> window, bool blockEvents = true);
		~GuiLayer() override = default;

		void process() override;
		void onEvent(Graphics::InputEvent* ev) override;
		void save(const std::string& filePath) override;
		void load(const std::string& filePath) override;

		std::shared_ptr<Graphics::GuiWindowsManager> getWindowsManager();

	private:
		std::shared_ptr<Graphics::GuiWindowsManager> m_windowsManager = std::make_shared<Graphics::GuiWindowsManager>();
		std::shared_ptr<Graphics::GuiRenderer> m_renderer;
		bool m_blockEvents;
	};
}