#pragma once

#include <d3d12.h>
#include <array>
#include <vector>
#include <unordered_map>
#include "DX12Utils/dxutils.h"

namespace Threshold
{
    using uint = unsigned int;
    using uint2 = std::array<uint, 2>;
    using uint3 = std::array<uint, 3>;
    using uint4 = std::array<uint, 4>;

    using int2 = std::array<int, 2>;
    using int3 = std::array<int, 3>;
    using int4 = std::array<int, 4>;
    using float2 = std::array<float, 2>;
    using float3 = std::array<float, 3>;
    using float4 = std::array<float, 4>;
    using float4x4 = std::array<std::array<float, 4>, 4>;

    enum class NoiseTypes: int
    {
        White,
        Blue2D_Offset,
        Blue2D_Flipbook,
        Blue2D_Golden_Ratio,
        STBN_10,
        STBN_19,
        FAST_Blue_Exp_Separate,
        FAST_Blue_Exp_Product,
        FAST_Triangle_Blue_Exp_Separate,
        FAST_Triangle_Blue_Exp_Product,
        FAST_Binomial3x3_Exp,
        FAST_Box3x3_Exp,
        Blue_Tellusim_128_128_64,
        Blue_Stable_Fiddusion,
        R2,
        IGN,
        Bayer4,
        Bayer16,
        Bayer64,
        Bayer256,
        Round,
        Floor,
    };

    enum class SpatialFilters: int
    {
        None,
        Box,
        Gauss,
    };

    enum class TemporalFilters: int
    {
        None,
        Ema,
        EMA_plus_Clamp,
        Monte_Carlo,
    };

    inline const char* EnumToString(NoiseTypes value, bool displayString = false)
    {
        switch(value)
        {
            case NoiseTypes::White: return displayString ? "White" : "White";
            case NoiseTypes::Blue2D_Offset: return displayString ? "Blue2D Offset" : "Blue2D_Offset";
            case NoiseTypes::Blue2D_Flipbook: return displayString ? "Blue2D Flipbook" : "Blue2D_Flipbook";
            case NoiseTypes::Blue2D_Golden_Ratio: return displayString ? "Blue2D Golden Ratio" : "Blue2D_Golden_Ratio";
            case NoiseTypes::STBN_10: return displayString ? "STBN_10" : "STBN_10";
            case NoiseTypes::STBN_19: return displayString ? "STBN_19" : "STBN_19";
            case NoiseTypes::FAST_Blue_Exp_Separate: return displayString ? "FAST_Blue_Exp_Separate" : "FAST_Blue_Exp_Separate";
            case NoiseTypes::FAST_Blue_Exp_Product: return displayString ? "FAST_Blue_Exp_Product" : "FAST_Blue_Exp_Product";
            case NoiseTypes::FAST_Triangle_Blue_Exp_Separate: return displayString ? "FAST_Triangle_Blue_Exp_Separate" : "FAST_Triangle_Blue_Exp_Separate";
            case NoiseTypes::FAST_Triangle_Blue_Exp_Product: return displayString ? "FAST_Triangle_Blue_Exp_Product" : "FAST_Triangle_Blue_Exp_Product";
            case NoiseTypes::FAST_Binomial3x3_Exp: return displayString ? "FAST_Binomial3x3_Exp" : "FAST_Binomial3x3_Exp";
            case NoiseTypes::FAST_Box3x3_Exp: return displayString ? "FAST_Box3x3_Exp" : "FAST_Box3x3_Exp";
            case NoiseTypes::Blue_Tellusim_128_128_64: return displayString ? "Blue_Tellusim_128_128_64" : "Blue_Tellusim_128_128_64";
            case NoiseTypes::Blue_Stable_Fiddusion: return displayString ? "Blue_Stable_Fiddusion" : "Blue_Stable_Fiddusion";
            case NoiseTypes::R2: return displayString ? "R2" : "R2";
            case NoiseTypes::IGN: return displayString ? "IGN" : "IGN";
            case NoiseTypes::Bayer4: return displayString ? "Bayer4" : "Bayer4";
            case NoiseTypes::Bayer16: return displayString ? "Bayer16" : "Bayer16";
            case NoiseTypes::Bayer64: return displayString ? "Bayer64" : "Bayer64";
            case NoiseTypes::Bayer256: return displayString ? "Bayer256" : "Bayer256";
            case NoiseTypes::Round: return displayString ? "Round" : "Round";
            case NoiseTypes::Floor: return displayString ? "Floor" : "Floor";
            default: return nullptr;
        }
    }

    inline const char* EnumToString(SpatialFilters value, bool displayString = false)
    {
        switch(value)
        {
            case SpatialFilters::None: return displayString ? "None" : "None";
            case SpatialFilters::Box: return displayString ? "Box" : "Box";
            case SpatialFilters::Gauss: return displayString ? "Gauss" : "Gauss";
            default: return nullptr;
        }
    }

    inline const char* EnumToString(TemporalFilters value, bool displayString = false)
    {
        switch(value)
        {
            case TemporalFilters::None: return displayString ? "None" : "None";
            case TemporalFilters::Ema: return displayString ? "Ema" : "Ema";
            case TemporalFilters::EMA_plus_Clamp: return displayString ? "EMA + Clamp" : "EMA_plus_Clamp";
            case TemporalFilters::Monte_Carlo: return displayString ? "Monte Carlo" : "Monte_Carlo";
            default: return nullptr;
        }
    }

    struct ContextInternal
    {
        ID3D12QueryHeap* m_TimestampQueryHeap = nullptr;
        ID3D12Resource* m_TimestampReadbackBuffer = nullptr;

        static ID3D12CommandSignature* s_commandSignatureDispatch;

        struct Struct__ThresholdCB
        {
            unsigned int Animate1 = true;  // If false, does not animate, even if the global Animate variable is true
            unsigned int Animate2 = true;  // If false, does not animate, even if the global Animate variable is true
            unsigned int Animate3 = true;  // If false, does not animate, even if the global Animate variable is true
            unsigned int Animate4 = true;  // If false, does not animate, even if the global Animate variable is true
            unsigned int ExtendNoise1 = false;  // If true, uses a 2d low discrepancy shuffle to offset the texture every cycle, using all offsets before repeating, in a low discrepancy pattern.
            unsigned int ExtendNoise2 = false;  // If true, uses a 2d low discrepancy shuffle to offset the texture every cycle, using all offsets before repeating, in a low discrepancy pattern.
            unsigned int ExtendNoise3 = false;  // If true, uses a 2d low discrepancy shuffle to offset the texture every cycle, using all offsets before repeating, in a low discrepancy pattern.
            unsigned int ExtendNoise4 = false;  // If true, uses a 2d low discrepancy shuffle to offset the texture every cycle, using all offsets before repeating, in a low discrepancy pattern.
            int FrameIndex = 0;
            int NoiseType1 = (int)NoiseTypes::White;  // Upper Left
            int NoiseType2 = (int)NoiseTypes::White;  // Upper Right
            int NoiseType3 = (int)NoiseTypes::White;  // Lower Left
            int NoiseType4 = (int)NoiseTypes::White;  // Lower Right
            float Threshold = 1.000000f;
            float2 _padding0 = {0.0f,0.0f};  // Padding
        };

        struct Struct__SpatialFilterCB
        {
            int SpatialFilter1 = (int)SpatialFilters::None;
            int SpatialFilter2 = (int)SpatialFilters::None;
            int SpatialFilter3 = (int)SpatialFilters::None;
            int SpatialFilter4 = (int)SpatialFilters::None;
            float SpatialFilterParam1 = 1.000000f;  // Radius for box, sigma for gauss
            float SpatialFilterParam2 = 1.000000f;  // Radius for box, sigma for gauss
            float SpatialFilterParam3 = 1.000000f;  // Radius for box, sigma for gauss
            float SpatialFilterParam4 = 1.000000f;  // Radius for box, sigma for gauss
        };

        struct Struct__TemporalFilterCB
        {
            unsigned int Reset_Accumulation = false;
            int TemporalFilter1 = (int)TemporalFilters::None;
            int TemporalFilter2 = (int)TemporalFilters::None;
            int TemporalFilter3 = (int)TemporalFilters::None;
            int TemporalFilter4 = (int)TemporalFilters::None;
            float TemporalFilterAlpha1 = 0.100000f;  // Alpha for exponential moving average. 0.1 is common for TAA.
            float TemporalFilterAlpha2 = 0.100000f;  // Alpha for exponential moving average. 0.1 is common for TAA.
            float TemporalFilterAlpha3 = 0.100000f;  // Alpha for exponential moving average. 0.1 is common for TAA.
            float TemporalFilterAlpha4 = 0.100000f;  // Alpha for exponential moving average. 0.1 is common for TAA.
            float3 _padding0 = {0.0f,0.0f,0.0f};  // Padding
        };

        struct Struct__AdjustBrightnessCB
        {
            unsigned int Brighten1 = true;  // If true, multiplies result by BrightnessMultiplier
            unsigned int Brighten2 = true;  // If true, multiplies result by BrightnessMultiplier
            unsigned int Brighten3 = true;  // If true, multiplies result by BrightnessMultiplier
            unsigned int Brighten4 = true;  // If true, multiplies result by BrightnessMultiplier
            float BrightnessMultiplier = 1.000000f;  // Stippling can dim the result by adding dark pixels. This can brighten it.
            float3 _padding0 = {0.0f,0.0f,0.0f};  // Padding
        };

        // Variables
        int variable_FrameIndex = 0;

        ID3D12Resource* texture_Temp = nullptr;
        unsigned int texture_Temp_size[3] = { 0, 0, 0 };
        unsigned int texture_Temp_numMips = 0;
        DXGI_FORMAT texture_Temp_format = DXGI_FORMAT_UNKNOWN;
        static const D3D12_RESOURCE_FLAGS texture_Temp_flags =  D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
        const D3D12_RESOURCE_STATES c_texture_Temp_endingState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;

        ID3D12Resource* texture_TemporalAccum = nullptr;
        unsigned int texture_TemporalAccum_size[3] = { 0, 0, 0 };
        unsigned int texture_TemporalAccum_numMips = 0;
        DXGI_FORMAT texture_TemporalAccum_format = DXGI_FORMAT_UNKNOWN;
        static const D3D12_RESOURCE_FLAGS texture_TemporalAccum_flags =  D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
        const D3D12_RESOURCE_STATES c_texture_TemporalAccum_endingState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

        ID3D12Resource* texture__loadedTexture_0 = nullptr;
        unsigned int texture__loadedTexture_0_size[3] = { 0, 0, 0 };
        unsigned int texture__loadedTexture_0_numMips = 0;
        DXGI_FORMAT texture__loadedTexture_0_format = DXGI_FORMAT_UNKNOWN;
        static const D3D12_RESOURCE_FLAGS texture__loadedTexture_0_flags =  D3D12_RESOURCE_FLAG_NONE;
        const D3D12_RESOURCE_STATES c_texture__loadedTexture_0_endingState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;

        ID3D12Resource* texture__loadedTexture_1 = nullptr;
        unsigned int texture__loadedTexture_1_size[3] = { 0, 0, 0 };
        unsigned int texture__loadedTexture_1_numMips = 0;
        DXGI_FORMAT texture__loadedTexture_1_format = DXGI_FORMAT_UNKNOWN;
        static const D3D12_RESOURCE_FLAGS texture__loadedTexture_1_flags =  D3D12_RESOURCE_FLAG_NONE;
        const D3D12_RESOURCE_STATES c_texture__loadedTexture_1_endingState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;

        ID3D12Resource* texture__loadedTexture_2 = nullptr;
        unsigned int texture__loadedTexture_2_size[3] = { 0, 0, 0 };
        unsigned int texture__loadedTexture_2_numMips = 0;
        DXGI_FORMAT texture__loadedTexture_2_format = DXGI_FORMAT_UNKNOWN;
        static const D3D12_RESOURCE_FLAGS texture__loadedTexture_2_flags =  D3D12_RESOURCE_FLAG_NONE;
        const D3D12_RESOURCE_STATES c_texture__loadedTexture_2_endingState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;

        ID3D12Resource* texture__loadedTexture_3 = nullptr;
        unsigned int texture__loadedTexture_3_size[3] = { 0, 0, 0 };
        unsigned int texture__loadedTexture_3_numMips = 0;
        DXGI_FORMAT texture__loadedTexture_3_format = DXGI_FORMAT_UNKNOWN;
        static const D3D12_RESOURCE_FLAGS texture__loadedTexture_3_flags =  D3D12_RESOURCE_FLAG_NONE;
        const D3D12_RESOURCE_STATES c_texture__loadedTexture_3_endingState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;

        ID3D12Resource* texture__loadedTexture_4 = nullptr;
        unsigned int texture__loadedTexture_4_size[3] = { 0, 0, 0 };
        unsigned int texture__loadedTexture_4_numMips = 0;
        DXGI_FORMAT texture__loadedTexture_4_format = DXGI_FORMAT_UNKNOWN;
        static const D3D12_RESOURCE_FLAGS texture__loadedTexture_4_flags =  D3D12_RESOURCE_FLAG_NONE;
        const D3D12_RESOURCE_STATES c_texture__loadedTexture_4_endingState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;

        ID3D12Resource* texture__loadedTexture_5 = nullptr;
        unsigned int texture__loadedTexture_5_size[3] = { 0, 0, 0 };
        unsigned int texture__loadedTexture_5_numMips = 0;
        DXGI_FORMAT texture__loadedTexture_5_format = DXGI_FORMAT_UNKNOWN;
        static const D3D12_RESOURCE_FLAGS texture__loadedTexture_5_flags =  D3D12_RESOURCE_FLAG_NONE;
        const D3D12_RESOURCE_STATES c_texture__loadedTexture_5_endingState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;

        ID3D12Resource* texture__loadedTexture_6 = nullptr;
        unsigned int texture__loadedTexture_6_size[3] = { 0, 0, 0 };
        unsigned int texture__loadedTexture_6_numMips = 0;
        DXGI_FORMAT texture__loadedTexture_6_format = DXGI_FORMAT_UNKNOWN;
        static const D3D12_RESOURCE_FLAGS texture__loadedTexture_6_flags =  D3D12_RESOURCE_FLAG_NONE;
        const D3D12_RESOURCE_STATES c_texture__loadedTexture_6_endingState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;

        ID3D12Resource* texture__loadedTexture_7 = nullptr;
        unsigned int texture__loadedTexture_7_size[3] = { 0, 0, 0 };
        unsigned int texture__loadedTexture_7_numMips = 0;
        DXGI_FORMAT texture__loadedTexture_7_format = DXGI_FORMAT_UNKNOWN;
        static const D3D12_RESOURCE_FLAGS texture__loadedTexture_7_flags =  D3D12_RESOURCE_FLAG_NONE;
        const D3D12_RESOURCE_STATES c_texture__loadedTexture_7_endingState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;

        ID3D12Resource* texture__loadedTexture_8 = nullptr;
        unsigned int texture__loadedTexture_8_size[3] = { 0, 0, 0 };
        unsigned int texture__loadedTexture_8_numMips = 0;
        DXGI_FORMAT texture__loadedTexture_8_format = DXGI_FORMAT_UNKNOWN;
        static const D3D12_RESOURCE_FLAGS texture__loadedTexture_8_flags =  D3D12_RESOURCE_FLAG_NONE;
        const D3D12_RESOURCE_STATES c_texture__loadedTexture_8_endingState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;

        ID3D12Resource* texture__loadedTexture_9 = nullptr;
        unsigned int texture__loadedTexture_9_size[3] = { 0, 0, 0 };
        unsigned int texture__loadedTexture_9_numMips = 0;
        DXGI_FORMAT texture__loadedTexture_9_format = DXGI_FORMAT_UNKNOWN;
        static const D3D12_RESOURCE_FLAGS texture__loadedTexture_9_flags =  D3D12_RESOURCE_FLAG_NONE;
        const D3D12_RESOURCE_STATES c_texture__loadedTexture_9_endingState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;

        ID3D12Resource* texture__loadedTexture_10 = nullptr;
        unsigned int texture__loadedTexture_10_size[3] = { 0, 0, 0 };
        unsigned int texture__loadedTexture_10_numMips = 0;
        DXGI_FORMAT texture__loadedTexture_10_format = DXGI_FORMAT_UNKNOWN;
        static const D3D12_RESOURCE_FLAGS texture__loadedTexture_10_flags =  D3D12_RESOURCE_FLAG_NONE;
        const D3D12_RESOURCE_STATES c_texture__loadedTexture_10_endingState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;

        ID3D12Resource* texture__loadedTexture_11 = nullptr;
        unsigned int texture__loadedTexture_11_size[3] = { 0, 0, 0 };
        unsigned int texture__loadedTexture_11_numMips = 0;
        DXGI_FORMAT texture__loadedTexture_11_format = DXGI_FORMAT_UNKNOWN;
        static const D3D12_RESOURCE_FLAGS texture__loadedTexture_11_flags =  D3D12_RESOURCE_FLAG_NONE;
        const D3D12_RESOURCE_STATES c_texture__loadedTexture_11_endingState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;

        Struct__ThresholdCB constantBuffer__ThresholdCB_cpu;
        ID3D12Resource* constantBuffer__ThresholdCB = nullptr;

        static ID3D12PipelineState* computeShader_Threshold_pso;
        static ID3D12RootSignature* computeShader_Threshold_rootSig;

        Struct__SpatialFilterCB constantBuffer__SpatialFilterCB_cpu;
        ID3D12Resource* constantBuffer__SpatialFilterCB = nullptr;

        static ID3D12PipelineState* computeShader_SpatialFilter_pso;
        static ID3D12RootSignature* computeShader_SpatialFilter_rootSig;

        Struct__TemporalFilterCB constantBuffer__TemporalFilterCB_cpu;
        ID3D12Resource* constantBuffer__TemporalFilterCB = nullptr;

        static ID3D12PipelineState* computeShader_TemporalFilter_pso;
        static ID3D12RootSignature* computeShader_TemporalFilter_rootSig;

        Struct__AdjustBrightnessCB constantBuffer__AdjustBrightnessCB_cpu;
        ID3D12Resource* constantBuffer__AdjustBrightnessCB = nullptr;

        static ID3D12PipelineState* computeShader_AdjustBrightness_pso;
        static ID3D12RootSignature* computeShader_AdjustBrightness_rootSig;

        std::unordered_map<DX12Utils::SubResourceHeapAllocationInfo, int, DX12Utils::SubResourceHeapAllocationInfo> m_RTVCache;
        std::unordered_map<DX12Utils::SubResourceHeapAllocationInfo, int, DX12Utils::SubResourceHeapAllocationInfo> m_DSVCache;

        // Freed on destruction of the context
        std::vector<ID3D12Resource*> m_managedResources;
    };
};
