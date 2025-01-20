#pragma once

#include "technique.h"

namespace Dither
{
    inline void ShowToolTip(const char* tooltip)
    {
        if (!tooltip || !tooltip[0])
            return;

        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 0, 255));
        ImGui::Text("[?]");
        ImGui::PopStyleColor();
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
            ImGui::SetTooltip("%s", tooltip);
    }

    void MakeUI(Context* context, ID3D12CommandQueue* commandQueue)
    {
        ImGui::PushID("gigi_Dither");

        context->m_input.variable_Reset_Accumulation = ImGui::Button("Reset_Accumulation");
        ImGui::InputInt("BitsPerColorChannel", &context->m_input.variable_BitsPerColorChannel, 0);
        ImGui::Checkbox("Animate", &context->m_input.variable_Animate);
        {
            static const char* labels[] = {
                "White",
                "Blue2D Offset",
                "Blue2D Flipbook",
                "Blue2D Golden Ratio",
                "STBN_10",
                "STBN_19",
                "FAST_Blue_Exp_Separate",
                "FAST_Blue_Exp_Product",
                "FAST_Triangle_Blue_Exp_Separate",
                "FAST_Triangle_Blue_Exp_Product",
                "FAST_Binomial3x3_Exp",
                "FAST_Box3x3_Exp",
                "Blue_Tellusim_128_128_64",
                "Blue_Stable_Fiddusion",
                "R2",
                "IGN",
                "Bayer",
                "Bayer Plus Half",
                "Round",
                "Floor",
                "White4",
                "White4 Plus Half",
                "White8",
                "White8 Plus Half",
                "White512",
                "White Triangular",
            };
            ImGui::Combo("NoiseType1", (int*)&context->m_input.variable_NoiseType1, labels, 26);
            ShowToolTip("Upper Left");
        }
        ImGui::Checkbox("ExtendNoise1", &context->m_input.variable_ExtendNoise1);
        ShowToolTip("If true, uses a 2d low discrepancy shuffle to offset the texture every cycle, using all offsets before repeating, in a low discrepancy pattern.");
        ImGui::Checkbox("SubtractiveDither1", &context->m_input.variable_SubtractiveDither1);
        ShowToolTip("If true, simulates subtractive dither by dequantizing after quantization, and subtracting dither value.");
        {
            static const char* labels[] = {
                "None",
                "Box",
                "Gauss",
            };
            ImGui::Combo("SpatialFilter1", (int*)&context->m_input.variable_SpatialFilter1, labels, 3);
        }
        ImGui::InputFloat("SpatialFilterParam1", &context->m_input.variable_SpatialFilterParam1);
        ShowToolTip("Radius for box, sigma for gauss");
        {
            static const char* labels[] = {
                "None",
                "EMA",
                "EMA + Clamp",
                "Monte Carlo",
            };
            ImGui::Combo("TemporalFilter1", (int*)&context->m_input.variable_TemporalFilter1, labels, 4);
        }
        ImGui::InputFloat("TemporalFilterAlpha1", &context->m_input.variable_TemporalFilterAlpha1);
        ShowToolTip("Alpha for exponential moving average. 0.1 is common for TAA.");
        ImGui::Checkbox("Animate1", &context->m_input.variable_Animate1);
        ShowToolTip("If false, does not animate, even if the global Animate variable is true");
        {
            static const char* labels[] = {
                "White",
                "Blue2D Offset",
                "Blue2D Flipbook",
                "Blue2D Golden Ratio",
                "STBN_10",
                "STBN_19",
                "FAST_Blue_Exp_Separate",
                "FAST_Blue_Exp_Product",
                "FAST_Triangle_Blue_Exp_Separate",
                "FAST_Triangle_Blue_Exp_Product",
                "FAST_Binomial3x3_Exp",
                "FAST_Box3x3_Exp",
                "Blue_Tellusim_128_128_64",
                "Blue_Stable_Fiddusion",
                "R2",
                "IGN",
                "Bayer",
                "Bayer Plus Half",
                "Round",
                "Floor",
                "White4",
                "White4 Plus Half",
                "White8",
                "White8 Plus Half",
                "White512",
                "White Triangular",
            };
            ImGui::Combo("NoiseType2", (int*)&context->m_input.variable_NoiseType2, labels, 26);
            ShowToolTip("Upper Right");
        }
        ImGui::Checkbox("ExtendNoise2", &context->m_input.variable_ExtendNoise2);
        ShowToolTip("If true, uses a 2d low discrepancy shuffle to offset the texture every cycle, using all offsets before repeating, in a low discrepancy pattern.");
        ImGui::Checkbox("SubtractiveDither2", &context->m_input.variable_SubtractiveDither2);
        ShowToolTip("If true, simulates subtractive dither by dequantizing after quantization, and subtracting dither value.");
        {
            static const char* labels[] = {
                "None",
                "Box",
                "Gauss",
            };
            ImGui::Combo("SpatialFilter2", (int*)&context->m_input.variable_SpatialFilter2, labels, 3);
        }
        ImGui::InputFloat("SpatialFilterParam2", &context->m_input.variable_SpatialFilterParam2);
        ShowToolTip("Radius for box, sigma for gauss");
        {
            static const char* labels[] = {
                "None",
                "EMA",
                "EMA + Clamp",
                "Monte Carlo",
            };
            ImGui::Combo("TemporalFilter2", (int*)&context->m_input.variable_TemporalFilter2, labels, 4);
        }
        ImGui::InputFloat("TemporalFilterAlpha2", &context->m_input.variable_TemporalFilterAlpha2);
        ShowToolTip("Alpha for exponential moving average. 0.1 is common for TAA.");
        ImGui::Checkbox("Animate2", &context->m_input.variable_Animate2);
        ShowToolTip("If false, does not animate, even if the global Animate variable is true");
        {
            static const char* labels[] = {
                "White",
                "Blue2D Offset",
                "Blue2D Flipbook",
                "Blue2D Golden Ratio",
                "STBN_10",
                "STBN_19",
                "FAST_Blue_Exp_Separate",
                "FAST_Blue_Exp_Product",
                "FAST_Triangle_Blue_Exp_Separate",
                "FAST_Triangle_Blue_Exp_Product",
                "FAST_Binomial3x3_Exp",
                "FAST_Box3x3_Exp",
                "Blue_Tellusim_128_128_64",
                "Blue_Stable_Fiddusion",
                "R2",
                "IGN",
                "Bayer",
                "Bayer Plus Half",
                "Round",
                "Floor",
                "White4",
                "White4 Plus Half",
                "White8",
                "White8 Plus Half",
                "White512",
                "White Triangular",
            };
            ImGui::Combo("NoiseType3", (int*)&context->m_input.variable_NoiseType3, labels, 26);
            ShowToolTip("Lower Left");
        }
        ImGui::Checkbox("ExtendNoise3", &context->m_input.variable_ExtendNoise3);
        ShowToolTip("If true, uses a 2d low discrepancy shuffle to offset the texture every cycle, using all offsets before repeating, in a low discrepancy pattern.");
        ImGui::Checkbox("SubtractiveDither3", &context->m_input.variable_SubtractiveDither3);
        ShowToolTip("If true, simulates subtractive dither by dequantizing after quantization, and subtracting dither value.");
        {
            static const char* labels[] = {
                "None",
                "Box",
                "Gauss",
            };
            ImGui::Combo("SpatialFilter3", (int*)&context->m_input.variable_SpatialFilter3, labels, 3);
        }
        ImGui::InputFloat("SpatialFilterParam3", &context->m_input.variable_SpatialFilterParam3);
        ShowToolTip("Radius for box, sigma for gauss");
        {
            static const char* labels[] = {
                "None",
                "EMA",
                "EMA + Clamp",
                "Monte Carlo",
            };
            ImGui::Combo("TemporalFilter3", (int*)&context->m_input.variable_TemporalFilter3, labels, 4);
        }
        ImGui::InputFloat("TemporalFilterAlpha3", &context->m_input.variable_TemporalFilterAlpha3);
        ShowToolTip("Alpha for exponential moving average. 0.1 is common for TAA.");
        ImGui::Checkbox("Animate3", &context->m_input.variable_Animate3);
        ShowToolTip("If false, does not animate, even if the global Animate variable is true");
        {
            static const char* labels[] = {
                "White",
                "Blue2D Offset",
                "Blue2D Flipbook",
                "Blue2D Golden Ratio",
                "STBN_10",
                "STBN_19",
                "FAST_Blue_Exp_Separate",
                "FAST_Blue_Exp_Product",
                "FAST_Triangle_Blue_Exp_Separate",
                "FAST_Triangle_Blue_Exp_Product",
                "FAST_Binomial3x3_Exp",
                "FAST_Box3x3_Exp",
                "Blue_Tellusim_128_128_64",
                "Blue_Stable_Fiddusion",
                "R2",
                "IGN",
                "Bayer",
                "Bayer Plus Half",
                "Round",
                "Floor",
                "White4",
                "White4 Plus Half",
                "White8",
                "White8 Plus Half",
                "White512",
                "White Triangular",
            };
            ImGui::Combo("NoiseType4", (int*)&context->m_input.variable_NoiseType4, labels, 26);
            ShowToolTip("Lower Right");
        }
        ImGui::Checkbox("ExtendNoise4", &context->m_input.variable_ExtendNoise4);
        ShowToolTip("If true, uses a 2d low discrepancy shuffle to offset the texture every cycle, using all offsets before repeating, in a low discrepancy pattern.");
        ImGui::Checkbox("SubtractiveDither4", &context->m_input.variable_SubtractiveDither4);
        ShowToolTip("If true, simulates subtractive dither by dequantizing after quantization, and subtracting dither value.");
        {
            static const char* labels[] = {
                "None",
                "Box",
                "Gauss",
            };
            ImGui::Combo("SpatialFilter4", (int*)&context->m_input.variable_SpatialFilter4, labels, 3);
        }
        ImGui::InputFloat("SpatialFilterParam4", &context->m_input.variable_SpatialFilterParam4);
        ShowToolTip("Radius for box, sigma for gauss");
        {
            static const char* labels[] = {
                "None",
                "EMA",
                "EMA + Clamp",
                "Monte Carlo",
            };
            ImGui::Combo("TemporalFilter4", (int*)&context->m_input.variable_TemporalFilter4, labels, 4);
        }
        ImGui::InputFloat("TemporalFilterAlpha4", &context->m_input.variable_TemporalFilterAlpha4);
        ShowToolTip("Alpha for exponential moving average. 0.1 is common for TAA.");
        ImGui::Checkbox("Animate4", &context->m_input.variable_Animate4);
        ShowToolTip("If false, does not animate, even if the global Animate variable is true");

        ImGui::Checkbox("Profile", &context->m_profile);
        if (context->m_profile)
        {
            int numEntries = 0;
            const ProfileEntry* entries = context->ReadbackProfileData(commandQueue, numEntries);
            if (ImGui::BeginTable("profiling", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
            {
                ImGui::TableSetupColumn("Label");
                ImGui::TableSetupColumn("CPU ms");
                ImGui::TableSetupColumn("GPU ms");
                ImGui::TableHeadersRow();
                float totalCpu = 0.0f;
                float totalGpu = 0.0f;
                for (int entryIndex = 0; entryIndex < numEntries; ++entryIndex)
                {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted(entries[entryIndex].m_label);
                    ImGui::TableNextColumn();
                    ImGui::Text("%0.3f", entries[entryIndex].m_cpu * 1000.0f);
                    ImGui::TableNextColumn();
                    ImGui::Text("%0.3f", entries[entryIndex].m_gpu * 1000.0f);
                    totalCpu += entries[entryIndex].m_cpu;
                    totalGpu += entries[entryIndex].m_gpu;
                }
                ImGui::EndTable();
            }
        }

        ImGui::PopID();
    }
};
