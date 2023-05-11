#include "pch.h"

#include "Layers.h"


namespace projectSolar::Layers
{
    Layer::Layer() :
        EventHandler(1)
    {
    }

    // Layers Manager
    void LayersManager::draw()
    {
        for (const auto& [id, layer] : m_attached)
        {
            layer->draw();
        }
    }
    bool LayersManager::onEvent(Graphics::InputEvent* ev)
    {
        for (auto it = m_attached.rbegin(); it != m_attached.rend(); ++it)
        {
            it->second->onEvent(ev);
            if (ev->handled)
            {
                return true;
            }
        }
        return false;
    }
}