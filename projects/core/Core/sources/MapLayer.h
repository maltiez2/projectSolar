#pragma once

#include "CentralRenderer.h"
#include "SimulationRunner.h"

namespace projectSolar
{
    class MapLayer
    {
    public:
        explicit MapLayer(Renderer& centralRenderer, Simulation::SimulationRunner& simulation);
        ~MapLayer();

        void draw(glm::mat4 mvp);

    private:
        struct Point
        {
            float x;
            float y;
            float z;
            uint32_t type;
        };

        const char* shaderFile = "resources/shaders/map.shader";

        Renderer& m_centralRenderer;
        Simulation::SimulationRunner& m_simulation;
        Shader m_shader;
        VertexBufferLayout m_layout;
        VertexArray m_vertexArray;
        std::vector<Point> m_buffer = {};
        std::vector<uint32_t> m_indices = {};

        VertexBuffer* m_vertexBuffer;
        IndexBuffer* m_indexBuffer;

        void updateData();
    };
}
