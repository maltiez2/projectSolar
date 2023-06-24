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
        ImGui::SliderFloat("Scale", &scale, 1e-5f, 1e2f, "%.5f", ImGuiSliderFlags_Logarithmic);
        ImGui::SliderFloat("Simulation load", &simLoad, 0.1f, 0.99f);
        ImGui::SliderFloat("Simulation rate", &simRate, 1e-3f, 1e0f, "%.4f", ImGuiSliderFlags_Logarithmic);
        ImGui::Checkbox("Follow selected", &followPlayer);
        ImGui::Checkbox("Run simulation", &runSimulation);
        showDemoWindow = ImGui::Button("Show demo window");
        ImGui::SliderInt("Primary objects", &primaryObj, 1, 32, "%d", ImGuiSliderFlags_Logarithmic);
        ImGui::SliderInt("Secondary objects per primary", &secondaryObj, 1, 32, "%d", ImGuiSliderFlags_Logarithmic);
        ImGui::SliderInt("Ternary objects", &ternaryObj, 1, 4096, "%d", ImGuiSliderFlags_Logarithmic);
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
