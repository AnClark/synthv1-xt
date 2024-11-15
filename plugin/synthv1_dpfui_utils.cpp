#include "synthv1_config.h"
#include "synthv1_dpfui.h"
#include "synthv1_param.h"

#include <random>


#if 0
void RenderCenteredText(const char* text)
{
    // 设置文本区域的宽度
    ImGui::PushItemWidth(200); // 假设我们想要的文本区域宽度为200

    // 计算文本的宽度
    ImVec2 textSize = ImGui::CalcTextSize(text);

    // 计算居中位置
    float cursorPosX = ImGui::GetCursorPosX();
    float itemWidth = ImGui::GetItemWidth();
    float textPosX = cursorPosX + (itemWidth - textSize.x) / 2;

    // 调整光标位置
    ImGui::SetCursorPosX(textPosX);

    // 渲染文本
    ImGui::Text("%s", text);

    // 恢复之前的ItemWidth设置
    ImGui::PopItemWidth();
}
#endif

void SynthV1PluginUI::_randomParams()
{
	float p = 1.0f;

	synthv1_config *pConfig = synthv1_config::getInstance();
	if (pConfig)
		p = 0.01f * pConfig->fRandomizePercent;

#if 0
	if (QMessageBox::warning(this,
		tr("Warning"),
		tr("About to randomize current parameter values:\n\n"
		"-/+ %1%.\n\n"
		"Are you sure?").arg(100.0f * p),
		QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Cancel)
		return;
#endif

	std::default_random_engine re(::time(nullptr));

	for (uint32_t i = 0; i < synthv1::NUM_PARAMS; ++i) {
		const synthv1::ParamIndex index = synthv1::ParamIndex(i);
		// Filter out some non-randomizable parameters!...
		if (index == synthv1::DCO1_OCTAVE  ||
			index == synthv1::DCO1_TUNING  ||
			index == synthv1::DCO1_ENVTIME ||
			index == synthv1::DCF1_ENABLED ||
			index == synthv1::LFO1_ENABLED) 
			continue;
		if (index >= synthv1::OUT1_WIDTH && index < synthv1::DCO2_SHAPE1)
			continue;
		if (index == synthv1::DCO2_OCTAVE  ||
			index == synthv1::DCO2_TUNING  ||
			index == synthv1::DCO2_ENVTIME ||
			index == synthv1::DCF2_ENABLED ||
			index == synthv1::LFO2_ENABLED) 
			continue;
		if (index >= synthv1::OUT2_WIDTH)
			break;

		{
			std::normal_distribution<float> nd;
			const float q = p * (synthv1_param::paramMaxValue(index) - synthv1_param::paramMinValue(index));
			float fValue = fParamStorage[index];

			if (synthv1_param::paramFloat(index))
				fValue += 0.5f * q * nd(re);
			else
				fValue = std::round(fValue + q * nd(re));
			if (fValue < synthv1_param::paramMinValue(index))
				fValue = synthv1_param::paramMinValue(index);
			else if (fValue > synthv1_param::paramMaxValue(index))
				fValue = synthv1_param::paramMaxValue(index);

			setParameterValue(index, fValue);
			parameterChanged(index, fValue);
		}
	}
}
