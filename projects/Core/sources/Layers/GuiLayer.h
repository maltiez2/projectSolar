#pragma once

#include "Layer.h"
#include "Graphics.h"
#include "GameLogic/CommunicationManager.h"

#include <memory>


namespace projectSolar::Layers
{
	class GuiLayer : public Layer
	{
	public:
		GuiLayer(std::shared_ptr<Graphics::Window> window, bool blockEvents = true);
		~GuiLayer() override = default;

		void process() override;
		void onEvent(Graphics::InputEvent* ev) override;

		std::shared_ptr<Graphics::GuiWindowsManager> getWindowsManager();

	private:
		std::shared_ptr<Graphics::GuiWindowsManager> m_windowsManager = std::make_shared<Graphics::GuiWindowsManager>();
		Graphics::GuiRenderer m_renderer;
		bool m_blockEvents;
	};
}