#include "Windows.h"
#include "Graphics.h"

#include <string>

namespace projectSolar::Windows
{
    void Gravity::show()
    {
        ImGui::Begin("Gravity", &showFlag);
        ImGui::SliderInt("Granularity", &granularity, 1, 16, "%d");
        ImGui::SliderInt("Min task size", &minTaskSize, 1, 2048, "%d", ImGuiSliderFlags_Logarithmic);
        ImGui::End();
    }
}
