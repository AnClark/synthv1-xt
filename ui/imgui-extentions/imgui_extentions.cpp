#include "imgui_extentions.h"
#include "imgui.h"

void ImGuiExt::RenderCenteredText(const char* text, const float width)
{
    // 设置文本区域的宽度
    ImGui::PushItemWidth(width); // 假设我们想要的文本区域宽度为200

    // 计算文本的宽度
    ImVec2 textSize = ImGui::CalcTextSize(text);

    // 计算居中位置
    float cursorPosX = ImGui::GetCursorPosX();
    float itemWidth = ImGui::CalcItemWidth();
    float textPosX = cursorPosX + (itemWidth - textSize.x) / 2;

    // 调整光标位置
    ImGui::SetCursorPosX(textPosX);

    // 渲染文本
    ImGui::Text("%s", text);

    // 恢复之前的ItemWidth设置
    ImGui::PopItemWidth();
}
