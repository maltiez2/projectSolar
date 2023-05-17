#include "Windows.h"
#include "Graphics.h"

#include <string>

namespace projectSolar::Windows
{
    void PropulsionControl::show()
    {
        ImGui::Begin("Propulsion control", &showFlag);
        ImGui::SliderFloat("Direction", &direction, 0.0f, 2.0f * 3.1415f);
        ImGui::SliderFloat("Magnitude", &magnitude, 0.0f, 1.0f);
        ImGui::End();
    }

    void Debug::show()
    {
        ImGui::Begin("Debug", &showFlag);
        ImGui::Text("Objects are draggable");
        ImGui::SliderFloat("Scale", &scale, 0.2f, 20.0f);
        ImGui::Checkbox("Follow player", &followPlayer);
        ImGui::Checkbox("Run simulation", &runSimulation);
        showDemoWindow = ImGui::Button("Show demo window");
        generateDebugData = ImGui::Button("Generate debug data");
        saveData = ImGui::Button("Save");
        loadData = ImGui::Button("Load");
        const ImGuiIO& io = ImGui::GetIO();
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::Text(std::format("Simulation steps per frame {}", stepsPerFrame).c_str());
        ImGui::Text(std::format("Simulation ms per frame {}", 1000.0f * stepsPerFrame * secondsPerStep).c_str());
        ImGui::Text("Objects under cursor:");
        for (auto& objName : objUnderCursor)
        {
            ImGui::Text(std::format("    {}", objName).c_str());
        }
        ImGui::End();
    }
}
