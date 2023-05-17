#include "pch.h"

#include "Layer.h"

#include "InputEvents.h"


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
    void LayersManager::save(const std::string& filePath) const
    {

        for (const auto& [id, layer] : m_registered)
        {
            layer->save(std::format("{}.{}", filePath, id));
        }
    }
    void LayersManager::load(const std::string& filePath) const
    {
        for (const auto& [id, layer] : m_registered)
        {
            layer->load(std::format("{}.{}", filePath, id));
        }
    }
}