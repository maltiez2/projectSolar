#include "pch.h"

#include "Layer.h"


namespace projectSolar::Layers
{

    // Layers Manager
    void LayersManager::process()
    {
        for (const auto& [id, layer] : m_attached)
        {
            layer->process();
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