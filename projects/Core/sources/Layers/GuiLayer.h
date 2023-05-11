#pragma once

#include "Layers.h"
#include "Graphics.h"

#include <memory>


namespace projectSolar::Layers
{
	class GuiLayer : public Layer
	{
	public:
		GuiLayer(std::shared_ptr<Graphics::Window> window, std::shared_ptr<Graphics::GuiWindowsManager> guiWindows, bool blockEvents = true);
		~GuiLayer() = default;

		void draw() override;
		void onEvent(Graphics::InputEvent* ev) override;

		std::shared_ptr<Graphics::GuiWindowsManager> getWindowsManager();

	private:
		std::shared_ptr<Graphics::GuiWindowsManager> m_windowsManager;
		Graphics::GuiRenderer m_renderer;
		bool m_blockEvents;
	};
}