#include "../public/technique.h"
#include "DX12Utils/dxutils.h"
#include "DX12Utils/DelayedReleaseTracker.h"
#include "DX12Utils/HeapAllocationTracker.h"
#include "DX12Utils/TextureCache.h"

#include <vector>
#include <chrono>

namespace Threshold
{
    static std::vector<Context*> s_allContexts;

    static DX12Utils::Heap                  s_srvHeap;
    static DX12Utils::Heap                  s_rtvHeap;
    static DX12Utils::Heap                  s_dsvHeap;
    static DX12Utils::UploadBufferTracker   s_ubTracker;
    static DX12Utils::DelayedReleaseTracker s_delayedRelease;
    static DX12Utils::HeapAllocationTracker s_heapAllocationTrackerRTV;
    static DX12Utils::HeapAllocationTracker s_heapAllocationTrackerDSV;

    TLogFn Context::LogFn = [] (LogLevel level, const char* msg, ...) {};
    TPerfEventBeginFn Context::PerfEventBeginFn = [] (const char* name, ID3D12GraphicsCommandList* commandList, int index) {};
    TPerfEventEndFn Context::PerfEventEndFn = [] (ID3D12GraphicsCommandList* commandList) {};

    std::wstring Context::s_techniqueLocation = L"./";
    static unsigned int s_timerIndex = 0;

    ID3D12CommandSignature* ContextInternal::s_commandSignatureDispatch = nullptr;

    ID3D12PipelineState* ContextInternal::computeShader_Threshold_pso = nullptr;
    ID3D12RootSignature* ContextInternal::computeShader_Threshold_rootSig = nullptr;

    ID3D12PipelineState* ContextInternal::computeShader_SpatialFilter_pso = nullptr;
    ID3D12RootSignature* ContextInternal::computeShader_SpatialFilter_rootSig = nullptr;

    ID3D12PipelineState* ContextInternal::computeShader_TemporalFilter_pso = nullptr;
    ID3D12RootSignature* ContextInternal::computeShader_TemporalFilter_rootSig = nullptr;

    ID3D12PipelineState* ContextInternal::computeShader_AdjustBrightness_pso = nullptr;
    ID3D12RootSignature* ContextInternal::computeShader_AdjustBrightness_rootSig = nullptr;

    template <typename T>
    T Pow2GE(const T& A)
    {
        float f = std::log2(float(A));
        f = std::ceilf(f);
        return (T)std::pow(2.0f, f);
    }

    bool CreateShared(ID3D12Device* device)
    {

        // Compute Shader: Threshold
        {
            D3D12_STATIC_SAMPLER_DESC samplers[1];

            // linearWrapSampler
            samplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
            samplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
            samplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
            samplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
            samplers[0].MipLODBias  = 0;
            samplers[0].MaxAnisotropy  = 0;
            samplers[0].ComparisonFunc  = D3D12_COMPARISON_FUNC_NEVER;
            samplers[0].BorderColor  = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
            samplers[0].MinLOD = 0.0f;
            samplers[0].MaxLOD = D3D12_FLOAT32_MAX;
            samplers[0].ShaderRegister = 0;
            samplers[0].RegisterSpace = 0;
            samplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

            D3D12_DESCRIPTOR_RANGE ranges[14];

            // Input
            ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
            ranges[0].NumDescriptors = 1;
            ranges[0].BaseShaderRegister = 0;
            ranges[0].RegisterSpace = 0;
            ranges[0].OffsetInDescriptorsFromTableStart = 0;

            // Output
            ranges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
            ranges[1].NumDescriptors = 1;
            ranges[1].BaseShaderRegister = 0;
            ranges[1].RegisterSpace = 0;
            ranges[1].OffsetInDescriptorsFromTableStart = 1;

            // ThresholdMap
            ranges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
            ranges[2].NumDescriptors = 1;
            ranges[2].BaseShaderRegister = 1;
            ranges[2].RegisterSpace = 0;
            ranges[2].OffsetInDescriptorsFromTableStart = 2;

            // _loadedTexture_0
            ranges[3].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
            ranges[3].NumDescriptors = 1;
            ranges[3].BaseShaderRegister = 2;
            ranges[3].RegisterSpace = 0;
            ranges[3].OffsetInDescriptorsFromTableStart = 3;

            // _loadedTexture_1
            ranges[4].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
            ranges[4].NumDescriptors = 1;
            ranges[4].BaseShaderRegister = 3;
            ranges[4].RegisterSpace = 0;
            ranges[4].OffsetInDescriptorsFromTableStart = 4;

            // _loadedTexture_2
            ranges[5].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
            ranges[5].NumDescriptors = 1;
            ranges[5].BaseShaderRegister = 4;
            ranges[5].RegisterSpace = 0;
            ranges[5].OffsetInDescriptorsFromTableStart = 5;

            // _loadedTexture_3
            ranges[6].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
            ranges[6].NumDescriptors = 1;
            ranges[6].BaseShaderRegister = 5;
            ranges[6].RegisterSpace = 0;
            ranges[6].OffsetInDescriptorsFromTableStart = 6;

            // _loadedTexture_4
            ranges[7].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
            ranges[7].NumDescriptors = 1;
            ranges[7].BaseShaderRegister = 6;
            ranges[7].RegisterSpace = 0;
            ranges[7].OffsetInDescriptorsFromTableStart = 7;

            // _loadedTexture_5
            ranges[8].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
            ranges[8].NumDescriptors = 1;
            ranges[8].BaseShaderRegister = 7;
            ranges[8].RegisterSpace = 0;
            ranges[8].OffsetInDescriptorsFromTableStart = 8;

            // _loadedTexture_6
            ranges[9].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
            ranges[9].NumDescriptors = 1;
            ranges[9].BaseShaderRegister = 8;
            ranges[9].RegisterSpace = 0;
            ranges[9].OffsetInDescriptorsFromTableStart = 9;

            // _loadedTexture_7
            ranges[10].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
            ranges[10].NumDescriptors = 1;
            ranges[10].BaseShaderRegister = 9;
            ranges[10].RegisterSpace = 0;
            ranges[10].OffsetInDescriptorsFromTableStart = 10;

            // _loadedTexture_8
            ranges[11].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
            ranges[11].NumDescriptors = 1;
            ranges[11].BaseShaderRegister = 10;
            ranges[11].RegisterSpace = 0;
            ranges[11].OffsetInDescriptorsFromTableStart = 11;

            // _loadedTexture_9
            ranges[12].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
            ranges[12].NumDescriptors = 1;
            ranges[12].BaseShaderRegister = 11;
            ranges[12].RegisterSpace = 0;
            ranges[12].OffsetInDescriptorsFromTableStart = 12;

            // _ThresholdCB
            ranges[13].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
            ranges[13].NumDescriptors = 1;
            ranges[13].BaseShaderRegister = 0;
            ranges[13].RegisterSpace = 0;
            ranges[13].OffsetInDescriptorsFromTableStart = 13;

            if(!DX12Utils::MakeRootSig(device, ranges, 14, samplers, 1, &ContextInternal::computeShader_Threshold_rootSig, (c_debugNames ? L"Threshold" : nullptr), Context::LogFn))
                return false;

            D3D_SHADER_MACRO defines[] = {
                { "__GigiDispatchMultiply", "uint3(1,1,1)" },
                { "__GigiDispatchDivide", "uint3(1,1,1)" },
                { "__GigiDispatchPreAdd", "uint3(0,0,0)" },
                { "__GigiDispatchPostAdd", "uint3(0,0,0)" },
                { nullptr, nullptr }
            };

            if(!DX12Utils::MakeComputePSO_DXC(device, Context::s_techniqueLocation.c_str(), L"shaders/ThresholdCS.hlsl", "csmain", "cs_6_1", defines,
               ContextInternal::computeShader_Threshold_rootSig, &ContextInternal::computeShader_Threshold_pso, c_debugShaders, (c_debugNames ? L"Threshold" : nullptr), Context::LogFn))
                return false;
        }

        // Compute Shader: SpatialFilter
        {
            D3D12_STATIC_SAMPLER_DESC* samplers = nullptr;

            D3D12_DESCRIPTOR_RANGE ranges[3];

            // Input
            ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
            ranges[0].NumDescriptors = 1;
            ranges[0].BaseShaderRegister = 0;
            ranges[0].RegisterSpace = 0;
            ranges[0].OffsetInDescriptorsFromTableStart = 0;

            // Output
            ranges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
            ranges[1].NumDescriptors = 1;
            ranges[1].BaseShaderRegister = 0;
            ranges[1].RegisterSpace = 0;
            ranges[1].OffsetInDescriptorsFromTableStart = 1;

            // _SpatialFilterCB
            ranges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
            ranges[2].NumDescriptors = 1;
            ranges[2].BaseShaderRegister = 0;
            ranges[2].RegisterSpace = 0;
            ranges[2].OffsetInDescriptorsFromTableStart = 2;

            if(!DX12Utils::MakeRootSig(device, ranges, 3, samplers, 0, &ContextInternal::computeShader_SpatialFilter_rootSig, (c_debugNames ? L"SpatialFilter" : nullptr), Context::LogFn))
                return false;

            D3D_SHADER_MACRO defines[] = {
                { "__GigiDispatchMultiply", "uint3(1,1,1)" },
                { "__GigiDispatchDivide", "uint3(1,1,1)" },
                { "__GigiDispatchPreAdd", "uint3(0,0,0)" },
                { "__GigiDispatchPostAdd", "uint3(0,0,0)" },
                { nullptr, nullptr }
            };

            if(!DX12Utils::MakeComputePSO_DXC(device, Context::s_techniqueLocation.c_str(), L"shaders/SpatialFilterCS.hlsl", "csmain", "cs_6_1", defines,
               ContextInternal::computeShader_SpatialFilter_rootSig, &ContextInternal::computeShader_SpatialFilter_pso, c_debugShaders, (c_debugNames ? L"SpatialFilter" : nullptr), Context::LogFn))
                return false;
        }

        // Compute Shader: TemporalFilter
        {
            D3D12_STATIC_SAMPLER_DESC* samplers = nullptr;

            D3D12_DESCRIPTOR_RANGE ranges[4];

            // Input
            ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
            ranges[0].NumDescriptors = 1;
            ranges[0].BaseShaderRegister = 0;
            ranges[0].RegisterSpace = 0;
            ranges[0].OffsetInDescriptorsFromTableStart = 0;

            // Output
            ranges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
            ranges[1].NumDescriptors = 1;
            ranges[1].BaseShaderRegister = 0;
            ranges[1].RegisterSpace = 0;
            ranges[1].OffsetInDescriptorsFromTableStart = 1;

            // TemporalAccum
            ranges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
            ranges[2].NumDescriptors = 1;
            ranges[2].BaseShaderRegister = 1;
            ranges[2].RegisterSpace = 0;
            ranges[2].OffsetInDescriptorsFromTableStart = 2;

            // _TemporalFilterCB
            ranges[3].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
            ranges[3].NumDescriptors = 1;
            ranges[3].BaseShaderRegister = 0;
            ranges[3].RegisterSpace = 0;
            ranges[3].OffsetInDescriptorsFromTableStart = 3;

            if(!DX12Utils::MakeRootSig(device, ranges, 4, samplers, 0, &ContextInternal::computeShader_TemporalFilter_rootSig, (c_debugNames ? L"TemporalFilter" : nullptr), Context::LogFn))
                return false;

            D3D_SHADER_MACRO defines[] = {
                { "__GigiDispatchMultiply", "uint3(1,1,1)" },
                { "__GigiDispatchDivide", "uint3(1,1,1)" },
                { "__GigiDispatchPreAdd", "uint3(0,0,0)" },
                { "__GigiDispatchPostAdd", "uint3(0,0,0)" },
                { nullptr, nullptr }
            };

            if(!DX12Utils::MakeComputePSO_DXC(device, Context::s_techniqueLocation.c_str(), L"shaders/TemporalFilterCS.hlsl", "csmain", "cs_6_1", defines,
               ContextInternal::computeShader_TemporalFilter_rootSig, &ContextInternal::computeShader_TemporalFilter_pso, c_debugShaders, (c_debugNames ? L"TemporalFilter" : nullptr), Context::LogFn))
                return false;
        }

        // Compute Shader: AdjustBrightness
        {
            D3D12_STATIC_SAMPLER_DESC* samplers = nullptr;

            D3D12_DESCRIPTOR_RANGE ranges[2];

            // Color
            ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
            ranges[0].NumDescriptors = 1;
            ranges[0].BaseShaderRegister = 0;
            ranges[0].RegisterSpace = 0;
            ranges[0].OffsetInDescriptorsFromTableStart = 0;

            // _AdjustBrightnessCB
            ranges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
            ranges[1].NumDescriptors = 1;
            ranges[1].BaseShaderRegister = 0;
            ranges[1].RegisterSpace = 0;
            ranges[1].OffsetInDescriptorsFromTableStart = 1;

            if(!DX12Utils::MakeRootSig(device, ranges, 2, samplers, 0, &ContextInternal::computeShader_AdjustBrightness_rootSig, (c_debugNames ? L"AdjustBrightness" : nullptr), Context::LogFn))
                return false;

            D3D_SHADER_MACRO defines[] = {
                { "__GigiDispatchMultiply", "uint3(1,1,1)" },
                { "__GigiDispatchDivide", "uint3(1,1,1)" },
                { "__GigiDispatchPreAdd", "uint3(0,0,0)" },
                { "__GigiDispatchPostAdd", "uint3(0,0,0)" },
                { nullptr, nullptr }
            };

            if(!DX12Utils::MakeComputePSO_DXC(device, Context::s_techniqueLocation.c_str(), L"shaders/AdjustBrightnessCS.hlsl", "csmain", "cs_6_1", defines,
               ContextInternal::computeShader_AdjustBrightness_rootSig, &ContextInternal::computeShader_AdjustBrightness_pso, c_debugShaders, (c_debugNames ? L"AdjustBrightness" : nullptr), Context::LogFn))
                return false;
        }

        // Create heaps
        if (c_numSRVDescriptors > 0 && !CreateHeap(s_srvHeap, device, c_numSRVDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, Context::LogFn))
            return false;

        if (c_numRTVDescriptors > 0 && !CreateHeap(s_rtvHeap, device, c_numRTVDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, Context::LogFn))
            return false;

        if (c_numDSVDescriptors > 0 && !CreateHeap(s_dsvHeap, device, c_numDSVDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, Context::LogFn))
            return false;

        s_heapAllocationTrackerRTV.Init(s_rtvHeap.m_heap, c_numRTVDescriptors, (int)device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
        s_heapAllocationTrackerDSV.Init(s_dsvHeap.m_heap, c_numDSVDescriptors, (int)device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV));

        // create indirect dispatch command
        {
            D3D12_INDIRECT_ARGUMENT_DESC dispatchArg = {};
            dispatchArg.Type						 = D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH;

            D3D12_COMMAND_SIGNATURE_DESC dispatchDesc = {};
            dispatchDesc.ByteStride					  = sizeof(uint32_t) * 3;
            dispatchDesc.NumArgumentDescs			  = 1;
            dispatchDesc.pArgumentDescs				  = &dispatchArg;
            dispatchDesc.NodeMask					  = 0x0;

            device->CreateCommandSignature(
                &dispatchDesc,
                nullptr,
                IID_PPV_ARGS(&ContextInternal::s_commandSignatureDispatch));
        }

        return true;
    }

    void DestroyShared()
    {

        if(ContextInternal::computeShader_Threshold_pso)
        {
            s_delayedRelease.Add(ContextInternal::computeShader_Threshold_pso);
            ContextInternal::computeShader_Threshold_pso = nullptr;
        }

        if(ContextInternal::computeShader_Threshold_rootSig)
        {
            s_delayedRelease.Add(ContextInternal::computeShader_Threshold_rootSig);
            ContextInternal::computeShader_Threshold_rootSig = nullptr;
        }

        if(ContextInternal::computeShader_SpatialFilter_pso)
        {
            s_delayedRelease.Add(ContextInternal::computeShader_SpatialFilter_pso);
            ContextInternal::computeShader_SpatialFilter_pso = nullptr;
        }

        if(ContextInternal::computeShader_SpatialFilter_rootSig)
        {
            s_delayedRelease.Add(ContextInternal::computeShader_SpatialFilter_rootSig);
            ContextInternal::computeShader_SpatialFilter_rootSig = nullptr;
        }

        if(ContextInternal::computeShader_TemporalFilter_pso)
        {
            s_delayedRelease.Add(ContextInternal::computeShader_TemporalFilter_pso);
            ContextInternal::computeShader_TemporalFilter_pso = nullptr;
        }

        if(ContextInternal::computeShader_TemporalFilter_rootSig)
        {
            s_delayedRelease.Add(ContextInternal::computeShader_TemporalFilter_rootSig);
            ContextInternal::computeShader_TemporalFilter_rootSig = nullptr;
        }

        if(ContextInternal::computeShader_AdjustBrightness_pso)
        {
            s_delayedRelease.Add(ContextInternal::computeShader_AdjustBrightness_pso);
            ContextInternal::computeShader_AdjustBrightness_pso = nullptr;
        }

        if(ContextInternal::computeShader_AdjustBrightness_rootSig)
        {
            s_delayedRelease.Add(ContextInternal::computeShader_AdjustBrightness_rootSig);
            ContextInternal::computeShader_AdjustBrightness_rootSig = nullptr;
        }

        // Clear out heap trackers
        s_heapAllocationTrackerRTV.Release();
        s_heapAllocationTrackerDSV.Release();

        // Destroy Heaps
        DestroyHeap(s_srvHeap);
        DestroyHeap(s_rtvHeap);
        DestroyHeap(s_dsvHeap);

        // Destroy any upload buffers
        s_ubTracker.Release();

        // Finish any delayed release
        s_delayedRelease.Release();

        // Destroy indirect dispatch command
        if (ContextInternal::s_commandSignatureDispatch)
        {
            ContextInternal::s_commandSignatureDispatch->Release();
            ContextInternal::s_commandSignatureDispatch = nullptr;
        }
    }

    Context* CreateContext(ID3D12Device* device)
    {
        if (s_allContexts.size() == 0)
        {
            if (!CreateShared(device))
                return nullptr;
        }

        Context* ret = new Context;
        s_allContexts.push_back(ret);
        return ret;
    }

    void DestroyContext(Context* context)
    {
        s_allContexts.erase(std::remove(s_allContexts.begin(), s_allContexts.end(), context), s_allContexts.end());
        delete context;
        if (s_allContexts.size() == 0)
            DestroyShared();
    }

    void OnNewFrame(int framesInFlight)
    {
        s_delayedRelease.OnNewFrame(framesInFlight);
        s_ubTracker.OnNewFrame(framesInFlight);
        s_heapAllocationTrackerRTV.OnNewFrame(framesInFlight);
        s_heapAllocationTrackerDSV.OnNewFrame(framesInFlight);
    }

    int Context::GetContextCount()
    {
        return (int)s_allContexts.size();
    }

    Context* Context::GetContext(int index)
    {
        if (index >= 0 && index < GetContextCount())
            return s_allContexts[index];
        else
            return nullptr;
    }

    ID3D12Resource* Context::CreateManagedBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_FLAGS flags, const void* data, size_t size, const wchar_t* debugName, D3D12_RESOURCE_STATES desiredState)
    {
        // Make a buffer and have the context manage it
        ID3D12Resource* ret = DX12Utils::CreateBuffer(
            device,
            (unsigned int)size,
            flags,
            D3D12_RESOURCE_STATE_COPY_DEST,
            D3D12_HEAP_TYPE_DEFAULT,
            debugName,
            Context::LogFn
        );
        AddManagedResource(ret);

        // Copy the data to the buffer if we should
        if (data != nullptr && size > 0)
            UploadBufferData(device, commandList, ret, D3D12_RESOURCE_STATE_COPY_DEST, data, (unsigned int)size);

        // Do a resource transition if we should
        if (desiredState != D3D12_RESOURCE_STATE_COPY_DEST)
        {
            D3D12_RESOURCE_BARRIER barrier;

            barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            barrier.Transition.pResource = ret;
            barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
            barrier.Transition.StateAfter = desiredState;
            barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

            commandList->ResourceBarrier(1, &barrier);
        }

        // return the resource
        return ret;
    }

    ID3D12Resource* Context::CreateManagedTexture(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_FLAGS flags, DXGI_FORMAT format, const unsigned int size[3], unsigned int numMips, DX12Utils::ResourceType resourceType, const void* initialData, const wchar_t* debugName, D3D12_RESOURCE_STATES desiredState)
    {
        // Create a texture
        ID3D12Resource* ret = DX12Utils::CreateTexture(device, size, numMips, format, flags, D3D12_RESOURCE_STATE_COPY_DEST, resourceType, debugName, Context::LogFn);
        AddManagedResource(ret);

        // copy initial data in, if we should
        if (initialData != nullptr)
        {
            DX12Utils::DXGI_FORMAT_Info formatInfo = DX12Utils::Get_DXGI_FORMAT_Info(format, Context::LogFn);
            UploadTextureData(device, commandList, ret, D3D12_RESOURCE_STATE_COPY_DEST, initialData, size[0] * formatInfo.bytesPerPixel);
        }

        // Put the resource into the desired state
        if (desiredState != D3D12_RESOURCE_STATE_COPY_DEST)
        {
            D3D12_RESOURCE_BARRIER barrier;

            barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            barrier.Transition.pResource = ret;
            barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
            barrier.Transition.StateAfter = desiredState;
            barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

            commandList->ResourceBarrier(1, &barrier);
        }

        return ret;
    }

    ID3D12Resource* Context::CreateManagedTextureAndClear(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_FLAGS flags, DXGI_FORMAT format, const unsigned int size[3], unsigned int numMips, DX12Utils::ResourceType resourceType, void* clearValue, size_t clearValueSize, const wchar_t* debugName, D3D12_RESOURCE_STATES desiredState)
    {
        // Make sure the clear value is the correct size
        DX12Utils::DXGI_FORMAT_Info formatInfo = DX12Utils::Get_DXGI_FORMAT_Info(format, Context::LogFn);
        if (clearValue != nullptr && clearValueSize > 0 && clearValueSize != formatInfo.bytesPerPixel)
            return nullptr;

        // Copy data into the resource
        std::vector<unsigned char> expandedClearValue;
        void* initialData = nullptr;
        if (clearValue != nullptr && clearValueSize > 0)
        {
            expandedClearValue.resize(size[0] * size[1] * size[2] * formatInfo.bytesPerPixel);
            unsigned char* dest = expandedClearValue.data();
            for (size_t i = 0; i < size[0] * size[1] * size[2]; ++i)
            {
                memcpy(dest, clearValue, formatInfo.bytesPerPixel);
                dest += formatInfo.bytesPerPixel;
            }
            initialData = expandedClearValue.data();
        }

        // make and return the texture
        return CreateManagedTexture(device, commandList, flags, format, size, numMips, resourceType, initialData, debugName, desiredState);
    }

    ID3D12Resource* Context::CreateManagedTextureFromFile(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_FLAGS flags, DXGI_FORMAT format, DX12Utils::ResourceType resourceType, const char* fileName, bool sourceIsSRGB, unsigned int size[3], const wchar_t* debugName, D3D12_RESOURCE_STATES desiredState)
    {
        // Get the desired channel type
        DX12Utils::DXGI_FORMAT_Info formatInfo = DX12Utils::Get_DXGI_FORMAT_Info(format, Context::LogFn);
        DX12Utils::TextureCache::Type desiredChannelType = DX12Utils::TextureCache::Type::U8;
        if (formatInfo.channelType == DX12Utils::DXGI_FORMAT_Info::ChannelType::_uint8_t)
            desiredChannelType = DX12Utils::TextureCache::Type::U8;
        else if (formatInfo.channelType == DX12Utils::DXGI_FORMAT_Info::ChannelType::_float)
            desiredChannelType = DX12Utils::TextureCache::Type::F32;
        else
            return nullptr;

        if (resourceType == DX12Utils::ResourceType::Texture2D)
        {
            // Load the texture and convert as necessary
            DX12Utils::TextureCache::Texture texture = DX12Utils::TextureCache::GetAs(fileName, sourceIsSRGB, desiredChannelType, formatInfo.sRGB, formatInfo.channelCount);
            if (!texture.Valid())
                return nullptr;

            // store off image properties
            size[0] = texture.width;
            size[1] = texture.height;
            size[2] = 1;

            // make and return the texture
            return CreateManagedTexture(device, commandList, flags, format, size, 1, resourceType, texture.pixels.data(), debugName, desiredState);
        }
        else if (resourceType == DX12Utils::ResourceType::Texture2DArray ||
                 resourceType == DX12Utils::ResourceType::Texture3D ||
                 resourceType == DX12Utils::ResourceType::TextureCube)
        {
            static const char* c_cubeMapNames[] =
            {
                "Right",
                "Left",
                "Up",
                "Down",
                "Front",
                "Back"
            };

            bool useCubeMapNames = (resourceType == DX12Utils::ResourceType::TextureCube && strstr(fileName, "%s") != nullptr);
            bool hasPercentI = strstr(fileName, "%i") != nullptr;
            if (!useCubeMapNames && !hasPercentI)
                return nullptr;

            std::vector<DX12Utils::TextureCache::Texture> loadedTextureSlices;

            // Load multiple textures
            int textureIndex = -1;
            while (1)
            {
                textureIndex++;
                char indexedFileName[1024];

                if (useCubeMapNames)
                    sprintf_s(indexedFileName, fileName, c_cubeMapNames[textureIndex]);
                else
                    sprintf_s(indexedFileName, fileName, textureIndex);

                // Load the texture and convert as necessary
                DX12Utils::TextureCache::Texture loadedTextureSlice = DX12Utils::TextureCache::GetAs(indexedFileName, sourceIsSRGB, desiredChannelType, formatInfo.sRGB, formatInfo.channelCount);
                if (!loadedTextureSlice.Valid())
                {
                    if (textureIndex == 0)
                        return nullptr;
                    break;
                }

                // make sure the textures are the same size
                if (textureIndex > 0 && (loadedTextureSlice.width != loadedTextureSlices[0].width || loadedTextureSlice.height != loadedTextureSlices[0].height))
                    return nullptr;

                loadedTextureSlices.push_back(loadedTextureSlice);
            }

            // store the texture size
            size[0] = loadedTextureSlices[0].width;
            size[1] = loadedTextureSlices[0].height;
            size[2] = (unsigned int)loadedTextureSlices.size();

            // gather up all pixels into a contiguous chunk of memory
            std::vector<unsigned char> allPixels;
            for (const DX12Utils::TextureCache::Texture& texture : loadedTextureSlices)
                allPixels.insert(allPixels.end(), texture.pixels.begin(), texture.pixels.end());

            // make and return the texture
            return CreateManagedTexture(device, commandList, flags, format, size, 1, resourceType, allPixels.data(), debugName, desiredState);
        }
        else
            return nullptr;
    }

    void Context::UploadTextureData(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, ID3D12Resource* texture, D3D12_RESOURCE_STATES textureState, const void* data, unsigned int unalignedPitch)
    {
        // Get information about the texture
        int alignedPitch = ALIGN(D3D12_TEXTURE_DATA_PITCH_ALIGNMENT, unalignedPitch);
        D3D12_RESOURCE_DESC textureDesc = texture->GetDesc();

        // transition the resource to copy dest if it isn't already
        if (textureState != D3D12_RESOURCE_STATE_COPY_DEST)
        {
            D3D12_RESOURCE_BARRIER barrier;

            barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            barrier.Transition.pResource = texture;
            barrier.Transition.StateBefore = textureState;
            barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
            barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

            commandList->ResourceBarrier(1, &barrier);
        }

        // 3d textures do a single copy because it's a single sub resource.
        if (textureDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE3D)
        {
            // Get the upload buffer
            DX12Utils::UploadBufferTracker::UploadBufferTracker::Buffer* uploadBuffer = s_ubTracker.GetBuffer(device, alignedPitch * textureDesc.Height * textureDesc.DepthOrArraySize, Context::LogFn, false);

            // Map, copy, unmap
            {
                unsigned char* dest = nullptr;
                D3D12_RANGE readRange = { 0, 0 };
                HRESULT hr = uploadBuffer->buffer->Map(0, &readRange, (void**)&dest);
                if (FAILED(hr))
                {
                    Context::LogFn(LogLevel::Error, "Could not map upload buffer.");
                }
                else
                {
                    const unsigned char* src = (const unsigned char*)data;
                    for (int iz = 0; iz < textureDesc.DepthOrArraySize; ++iz)
                    {
                        for (int iy = 0; iy < (int)textureDesc.Height; ++iy)
                        {
                            memcpy(dest, src, unalignedPitch);
                            src += unalignedPitch;
                            dest += alignedPitch;
                        }
                    }

                    uploadBuffer->buffer->Unmap(0, nullptr);
                }
            }

            // copy the upload buffer into the texture
            {
                unsigned char layoutMem[sizeof(D3D12_PLACED_SUBRESOURCE_FOOTPRINT) + sizeof(UINT) + sizeof(UINT64)];
                D3D12_PLACED_SUBRESOURCE_FOOTPRINT* layout = (D3D12_PLACED_SUBRESOURCE_FOOTPRINT*)layoutMem;
                device->GetCopyableFootprints(&textureDesc, 0, 1, 0, layout, nullptr, nullptr, nullptr);

                D3D12_TEXTURE_COPY_LOCATION src = {};
                src.pResource = uploadBuffer->buffer;
                src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
                src.PlacedFootprint = *layout;

                D3D12_TEXTURE_COPY_LOCATION dest = {};
                dest.pResource = texture;
                dest.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
                dest.SubresourceIndex = 0;

                commandList->CopyTextureRegion(&dest, 0, 0, 0, &src, nullptr);
            }
        }
        // 2d array textures do a copy for each slice
        else if (textureDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)
        {
            for (int iz = 0; iz < textureDesc.DepthOrArraySize; ++iz)
            {
                // Get the upload buffer
                DX12Utils::UploadBufferTracker::Buffer* uploadBuffer = s_ubTracker.GetBuffer(device, alignedPitch * textureDesc.Height, Context::LogFn, false);

                // Map, copy, unmap
                {
                    unsigned char* dest = nullptr;
                    D3D12_RANGE readRange = { 0, 0 };
                    HRESULT hr = uploadBuffer->buffer->Map(0, &readRange, (void**)&dest);
                    if (FAILED(hr))
                    {
                        Context::LogFn(LogLevel::Error, "Could not map upload buffer.");
                    }
                    else
                    {
                        const unsigned char* src = &((const unsigned char*)data)[unalignedPitch * textureDesc.Height * iz];
                        for (int iy = 0; iy < (int)textureDesc.Height; ++iy)
                        {
                            memcpy(dest, src, unalignedPitch);
                            src += unalignedPitch;
                            dest += alignedPitch;
                        }

                        uploadBuffer->buffer->Unmap(0, nullptr);
                    }
                }

                 // copy the upload buffer into the texture
                 {
                     unsigned char layoutMem[sizeof(D3D12_PLACED_SUBRESOURCE_FOOTPRINT) + sizeof(UINT) + sizeof(UINT64)];
                     D3D12_PLACED_SUBRESOURCE_FOOTPRINT* layout = (D3D12_PLACED_SUBRESOURCE_FOOTPRINT*)layoutMem;
                     device->GetCopyableFootprints(&textureDesc, 0, 1, 0, layout, nullptr, nullptr, nullptr);

                     D3D12_TEXTURE_COPY_LOCATION src = {};
                     src.pResource = uploadBuffer->buffer;
                     src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
                     src.PlacedFootprint = *layout;

                     D3D12_TEXTURE_COPY_LOCATION dest = {};
                     dest.pResource = texture;
                     dest.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
                     dest.SubresourceIndex = iz;

                     commandList->CopyTextureRegion(&dest, 0, 0, 0, &src, nullptr);
                 }
            }
        }
        else
        {
            Context::LogFn(LogLevel::Error, "Unhandled texture dimension.");
        }

        // transition the resource back to what it was
        if (textureState != D3D12_RESOURCE_STATE_COPY_DEST)
        {
            D3D12_RESOURCE_BARRIER barrier;

            barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            barrier.Transition.pResource = texture;
            barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
            barrier.Transition.StateAfter = textureState;
            barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

            commandList->ResourceBarrier(1, &barrier);
        }
    }

    void Context::UploadBufferData(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, ID3D12Resource* buffer, D3D12_RESOURCE_STATES bufferState, const void* data, unsigned int dataSize)
    {
        // Get the upload buffer
        DX12Utils::UploadBufferTracker::UploadBufferTracker::Buffer* uploadBuffer = s_ubTracker.GetBuffer(device, dataSize, Context::LogFn, false);

        // copy cpu data to the upload buffer
        {
            void* start = nullptr;
            HRESULT hr = uploadBuffer->buffer->Map(0, nullptr, reinterpret_cast<void**>(&start));
            if(hr)
            {
                Context::LogFn(LogLevel::Error, "Could not map upload buffer");
                return;
            }

            memcpy(start, data, dataSize);

            uploadBuffer->buffer->Unmap(0, nullptr);
        }

        // transition the resource to copy dest if it isn't already
        if (bufferState != D3D12_RESOURCE_STATE_COPY_DEST)
        {
            D3D12_RESOURCE_BARRIER barrier;

            barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            barrier.Transition.pResource = buffer;
            barrier.Transition.StateBefore = bufferState;
            barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
            barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

            commandList->ResourceBarrier(1, &barrier);
        }

        // copy the resource
        commandList->CopyResource(buffer, uploadBuffer->buffer);

        // transition the resource back to what it was
        if (bufferState != D3D12_RESOURCE_STATE_COPY_DEST)
        {
            D3D12_RESOURCE_BARRIER barrier;

            barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            barrier.Transition.pResource = buffer;
            barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
            barrier.Transition.StateAfter = bufferState;
            barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

            commandList->ResourceBarrier(1, &barrier);
        }
    }

    int Context::GetRTV(ID3D12Device* device, ID3D12Resource* resource, DXGI_FORMAT format, D3D12_RTV_DIMENSION dimension, int arrayIndex, int mipIndex, const char* debugName)
    {
        // Make the key
        DX12Utils::SubResourceHeapAllocationInfo key;
        key.resource = resource;
        key.arrayIndex = arrayIndex;
        key.mipIndex = mipIndex;

        // If it already exists, use it
        auto it = m_internal.m_RTVCache.find(key);
        if (it != m_internal.m_RTVCache.end())
            return it->second;

        // Allocate an RTV index
        int rtvIndex = -1;
        if (!s_heapAllocationTrackerRTV.Allocate(rtvIndex, debugName))
            return -1;

        // Create the RTV
        if (!DX12Utils::CreateRTV(device, resource, s_heapAllocationTrackerRTV.GetCPUHandle(rtvIndex), format, dimension, arrayIndex, mipIndex))
        {
            s_heapAllocationTrackerRTV.Free(rtvIndex);
            return -1;
        }

        // store the result
        m_internal.m_RTVCache[key] = rtvIndex;
        return rtvIndex;
    }

    int Context::GetDSV(ID3D12Device* device, ID3D12Resource* resource, DXGI_FORMAT format, D3D12_DSV_DIMENSION dimension, int arrayIndex, int mipIndex, const char* debugName)
    {
	    // Make the key
        DX12Utils::SubResourceHeapAllocationInfo key;
        key.resource = resource;
        key.arrayIndex = arrayIndex;
        key.mipIndex = mipIndex;

	    // If it already exists, use it
	    auto it = m_internal.m_DSVCache.find(key);
	    if (it != m_internal.m_DSVCache.end())
            return it->second;

        // Allocate a DSV index
        int dsvIndex = -1;
        if (!s_heapAllocationTrackerDSV.Allocate(dsvIndex, debugName))
            return -1;

        // Create the DSV
        if (!DX12Utils::CreateDSV(device, resource, s_heapAllocationTrackerDSV.GetCPUHandle(dsvIndex), format, dimension, arrayIndex, mipIndex))
        {
            s_heapAllocationTrackerDSV.Free(dsvIndex);
            return -1;
        }

        // store the result
        m_internal.m_DSVCache[key] = dsvIndex;
        return dsvIndex;
    }

    const ProfileEntry* Context::ReadbackProfileData(ID3D12CommandQueue* commandQueue, int& numItems)
    {
        numItems = 0;

        if (!m_profile || !m_internal.m_TimestampReadbackBuffer)
            return nullptr;

        uint64_t GPUFrequency;
        commandQueue->GetTimestampFrequency(&GPUFrequency);
        double GPUTickDelta = 1.0 / static_cast<double>(GPUFrequency);

        D3D12_RANGE range;
        range.Begin = 0;
        range.End = ((4 + 1) * 2) * sizeof(uint64_t);

        uint64_t* timeStampBuffer = nullptr;
        m_internal.m_TimestampReadbackBuffer->Map(0, &range, (void**)&timeStampBuffer);

        m_profileData[numItems].m_gpu = float(GPUTickDelta * double(timeStampBuffer[numItems*2+2] - timeStampBuffer[numItems*2+1])); numItems++; // compute shader: Threshold
        m_profileData[numItems].m_gpu = float(GPUTickDelta * double(timeStampBuffer[numItems*2+2] - timeStampBuffer[numItems*2+1])); numItems++; // compute shader: SpatialFilter
        m_profileData[numItems].m_gpu = float(GPUTickDelta * double(timeStampBuffer[numItems*2+2] - timeStampBuffer[numItems*2+1])); numItems++; // compute shader: TemporalFilter
        m_profileData[numItems].m_gpu = float(GPUTickDelta * double(timeStampBuffer[numItems*2+2] - timeStampBuffer[numItems*2+1])); numItems++; // compute shader: AdjustBrightness
        m_profileData[numItems].m_gpu = float(GPUTickDelta * double(timeStampBuffer[numItems*2+1] - timeStampBuffer[0])); numItems++; // GPU total

        D3D12_RANGE emptyRange = {};
        m_internal.m_TimestampReadbackBuffer->Unmap(0, &emptyRange);

        return m_profileData;
    }

    Context::~Context()
    {
        for (const auto& pair : m_internal.m_RTVCache)
            s_heapAllocationTrackerRTV.Free(pair.second);
        m_internal.m_RTVCache.clear();

        for (const auto& pair : m_internal.m_DSVCache)
            s_heapAllocationTrackerDSV.Free(pair.second);
        m_internal.m_DSVCache.clear();

        for (ID3D12Resource* resource : m_internal.m_managedResources)
            resource->Release();
        m_internal.m_managedResources.clear();

        if(m_internal.m_TimestampQueryHeap)
        {
            m_internal.m_TimestampQueryHeap->Release();
            m_internal.m_TimestampQueryHeap = nullptr;
        }

        if(m_internal.m_TimestampReadbackBuffer)
        {
            m_internal.m_TimestampReadbackBuffer->Release();
            m_internal.m_TimestampReadbackBuffer = nullptr;
        }

        if(m_output.texture_Color)
        {
            s_delayedRelease.Add(m_output.texture_Color);
            m_output.texture_Color = nullptr;
        }

        if(m_internal.texture_Temp)
        {
            s_delayedRelease.Add(m_internal.texture_Temp);
            m_internal.texture_Temp = nullptr;
        }

        if(m_internal.texture_TemporalAccum)
        {
            s_delayedRelease.Add(m_internal.texture_TemporalAccum);
            m_internal.texture_TemporalAccum = nullptr;
        }

        if(m_internal.texture__loadedTexture_0)
        {
            s_delayedRelease.Add(m_internal.texture__loadedTexture_0);
            m_internal.texture__loadedTexture_0 = nullptr;
        }

        if(m_internal.texture__loadedTexture_1)
        {
            s_delayedRelease.Add(m_internal.texture__loadedTexture_1);
            m_internal.texture__loadedTexture_1 = nullptr;
        }

        if(m_internal.texture__loadedTexture_2)
        {
            s_delayedRelease.Add(m_internal.texture__loadedTexture_2);
            m_internal.texture__loadedTexture_2 = nullptr;
        }

        if(m_internal.texture__loadedTexture_3)
        {
            s_delayedRelease.Add(m_internal.texture__loadedTexture_3);
            m_internal.texture__loadedTexture_3 = nullptr;
        }

        if(m_internal.texture__loadedTexture_4)
        {
            s_delayedRelease.Add(m_internal.texture__loadedTexture_4);
            m_internal.texture__loadedTexture_4 = nullptr;
        }

        if(m_internal.texture__loadedTexture_5)
        {
            s_delayedRelease.Add(m_internal.texture__loadedTexture_5);
            m_internal.texture__loadedTexture_5 = nullptr;
        }

        if(m_internal.texture__loadedTexture_6)
        {
            s_delayedRelease.Add(m_internal.texture__loadedTexture_6);
            m_internal.texture__loadedTexture_6 = nullptr;
        }

        if(m_internal.texture__loadedTexture_7)
        {
            s_delayedRelease.Add(m_internal.texture__loadedTexture_7);
            m_internal.texture__loadedTexture_7 = nullptr;
        }

        if(m_internal.texture__loadedTexture_8)
        {
            s_delayedRelease.Add(m_internal.texture__loadedTexture_8);
            m_internal.texture__loadedTexture_8 = nullptr;
        }

        if(m_internal.texture__loadedTexture_9)
        {
            s_delayedRelease.Add(m_internal.texture__loadedTexture_9);
            m_internal.texture__loadedTexture_9 = nullptr;
        }

        // _ThresholdCB
        if (m_internal.constantBuffer__ThresholdCB)
        {
            s_delayedRelease.Add(m_internal.constantBuffer__ThresholdCB);
            m_internal.constantBuffer__ThresholdCB = nullptr;
        }

        // _SpatialFilterCB
        if (m_internal.constantBuffer__SpatialFilterCB)
        {
            s_delayedRelease.Add(m_internal.constantBuffer__SpatialFilterCB);
            m_internal.constantBuffer__SpatialFilterCB = nullptr;
        }

        // _TemporalFilterCB
        if (m_internal.constantBuffer__TemporalFilterCB)
        {
            s_delayedRelease.Add(m_internal.constantBuffer__TemporalFilterCB);
            m_internal.constantBuffer__TemporalFilterCB = nullptr;
        }

        // _AdjustBrightnessCB
        if (m_internal.constantBuffer__AdjustBrightnessCB)
        {
            s_delayedRelease.Add(m_internal.constantBuffer__AdjustBrightnessCB);
            m_internal.constantBuffer__AdjustBrightnessCB = nullptr;
        }
    }

    void Execute(Context* context, ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
    {
        // reset the timer index
        s_timerIndex = 0;

        ScopedPerfEvent scopedPerf("Threshold", commandList, 23);

        std::chrono::high_resolution_clock::time_point startPointCPUTechnique;
        if(context->m_profile)
        {
            startPointCPUTechnique = std::chrono::high_resolution_clock::now();
            if(context->m_internal.m_TimestampQueryHeap == nullptr)
            {
                D3D12_QUERY_HEAP_DESC QueryHeapDesc;
                QueryHeapDesc.Count = (4+1) * 2;
                QueryHeapDesc.NodeMask = 1;
                QueryHeapDesc.Type = D3D12_QUERY_HEAP_TYPE_TIMESTAMP;
                device->CreateQueryHeap(&QueryHeapDesc, IID_PPV_ARGS(&context->m_internal.m_TimestampQueryHeap));
                if (c_debugNames)
                    context->m_internal.m_TimestampQueryHeap->SetName(L"Threshold Time Stamp Query Heap");

                context->m_internal.m_TimestampReadbackBuffer = DX12Utils::CreateBuffer(device, sizeof(uint64_t) * (4+1) * 2, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_HEAP_TYPE_READBACK, (c_debugNames ? L"Threshold Time Stamp Query Heap" : nullptr), nullptr);
            }
            commandList->EndQuery(context->m_internal.m_TimestampQueryHeap, D3D12_QUERY_TYPE_TIMESTAMP, s_timerIndex++);
        }

        // Set variables
        if(context->m_input.variable_Animate)
            context->m_internal.variable_FrameIndex = context->m_internal.variable_FrameIndex + 1;
        if(context->m_input.variable_Auto_Brightness)
            context->m_input.variable_BrightnessMultiplier = 1.0f / context->m_input.variable_Threshold;

        if (!context->m_input.texture_Input)
        {
            Context::LogFn(LogLevel::Error, "Threshold: Imported texture \"Input\" is null.\n");
            return;
        }

        if (!context->m_input.texture_ThresholdMap)
        {
            Context::LogFn(LogLevel::Error, "Threshold: Imported texture \"ThresholdMap\" is null.\n");
            return;
        }

        // Make sure internally owned resources are created and are the right size and format
        context->EnsureResourcesCreated(device, commandList);

        // set the heaps
        ID3D12DescriptorHeap* heaps[] =
        {
            s_srvHeap.m_heap,
        };
        commandList->SetDescriptorHeaps(_countof(heaps), heaps);

        // Make sure imported resources are in the correct state
        {
            int barrierCount = 0;
            D3D12_RESOURCE_BARRIER barriers[2];

            if(context->m_input.texture_Input_state != D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE)
            {
                barriers[barrierCount].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                barriers[barrierCount].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                barriers[barrierCount].Transition.pResource = context->m_input.texture_Input;
                barriers[barrierCount].Transition.StateBefore = context->m_input.texture_Input_state;
                barriers[barrierCount].Transition.StateAfter = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
                barriers[barrierCount].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
                barrierCount++;
            }

            if(context->m_input.texture_ThresholdMap_state != D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE)
            {
                barriers[barrierCount].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                barriers[barrierCount].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                barriers[barrierCount].Transition.pResource = context->m_input.texture_ThresholdMap;
                barriers[barrierCount].Transition.StateBefore = context->m_input.texture_ThresholdMap_state;
                barriers[barrierCount].Transition.StateAfter = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
                barriers[barrierCount].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
                barrierCount++;
            }

            if(barrierCount > 0)
                commandList->ResourceBarrier(barrierCount, barriers);
        }

        // Shader Constants: _ThresholdCB
        {
            context->m_internal.constantBuffer__ThresholdCB_cpu.Animate1 = context->m_input.variable_Animate1;
            context->m_internal.constantBuffer__ThresholdCB_cpu.Animate2 = context->m_input.variable_Animate2;
            context->m_internal.constantBuffer__ThresholdCB_cpu.Animate3 = context->m_input.variable_Animate3;
            context->m_internal.constantBuffer__ThresholdCB_cpu.Animate4 = context->m_input.variable_Animate4;
            context->m_internal.constantBuffer__ThresholdCB_cpu.FrameIndex = context->m_internal.variable_FrameIndex;
            context->m_internal.constantBuffer__ThresholdCB_cpu.NoiseType1 = (int)context->m_input.variable_NoiseType1;
            context->m_internal.constantBuffer__ThresholdCB_cpu.NoiseType2 = (int)context->m_input.variable_NoiseType2;
            context->m_internal.constantBuffer__ThresholdCB_cpu.NoiseType3 = (int)context->m_input.variable_NoiseType3;
            context->m_internal.constantBuffer__ThresholdCB_cpu.NoiseType4 = (int)context->m_input.variable_NoiseType4;
            context->m_internal.constantBuffer__ThresholdCB_cpu.Threshold = context->m_input.variable_Threshold;
            DX12Utils::CopyConstantsCPUToGPU(s_ubTracker, device, commandList, context->m_internal.constantBuffer__ThresholdCB, context->m_internal.constantBuffer__ThresholdCB_cpu, Context::LogFn);
        }

        // Transition resources for the next action
        {
            D3D12_RESOURCE_BARRIER barriers[1];

            barriers[0].Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
            barriers[0].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            barriers[0].UAV.pResource = context->m_output.texture_Color;

            commandList->ResourceBarrier(1, barriers);
        }

        // Compute Shader: Threshold
        {
            ScopedPerfEvent scopedPerf("Compute Shader: Threshold", commandList, 0);
            std::chrono::high_resolution_clock::time_point startPointCPU;
            if(context->m_profile)
            {
                startPointCPU = std::chrono::high_resolution_clock::now();
                commandList->EndQuery(context->m_internal.m_TimestampQueryHeap, D3D12_QUERY_TYPE_TIMESTAMP, s_timerIndex++);
            }

            commandList->SetComputeRootSignature(ContextInternal::computeShader_Threshold_rootSig);
            commandList->SetPipelineState(ContextInternal::computeShader_Threshold_pso);

            DX12Utils::ResourceDescriptor descriptors[] = {
                { context->m_input.texture_Input, context->m_input.texture_Input_format, DX12Utils::AccessType::SRV, DX12Utils::ResourceType::Texture2D, false, 0, 0, 0 },
                { context->m_output.texture_Color, context->m_output.texture_Color_format, DX12Utils::AccessType::UAV, DX12Utils::ResourceType::Texture2D, false, 0, 0, 0 },
                { context->m_input.texture_ThresholdMap, context->m_input.texture_ThresholdMap_format, DX12Utils::AccessType::SRV, DX12Utils::ResourceType::Texture2D, false, 0, 0, 0 },
                { context->m_internal.texture__loadedTexture_0, context->m_internal.texture__loadedTexture_0_format, DX12Utils::AccessType::SRV, DX12Utils::ResourceType::Texture2D, false, 0, 0, 0 },
                { context->m_internal.texture__loadedTexture_1, context->m_internal.texture__loadedTexture_1_format, DX12Utils::AccessType::SRV, DX12Utils::ResourceType::Texture2DArray, false, 0, context->m_internal.texture__loadedTexture_1_size[2], 0 },
                { context->m_internal.texture__loadedTexture_2, context->m_internal.texture__loadedTexture_2_format, DX12Utils::AccessType::SRV, DX12Utils::ResourceType::Texture2DArray, false, 0, context->m_internal.texture__loadedTexture_2_size[2], 0 },
                { context->m_internal.texture__loadedTexture_3, context->m_internal.texture__loadedTexture_3_format, DX12Utils::AccessType::SRV, DX12Utils::ResourceType::Texture2DArray, false, 0, context->m_internal.texture__loadedTexture_3_size[2], 0 },
                { context->m_internal.texture__loadedTexture_4, context->m_internal.texture__loadedTexture_4_format, DX12Utils::AccessType::SRV, DX12Utils::ResourceType::Texture2DArray, false, 0, context->m_internal.texture__loadedTexture_4_size[2], 0 },
                { context->m_internal.texture__loadedTexture_5, context->m_internal.texture__loadedTexture_5_format, DX12Utils::AccessType::SRV, DX12Utils::ResourceType::Texture2DArray, false, 0, context->m_internal.texture__loadedTexture_5_size[2], 0 },
                { context->m_internal.texture__loadedTexture_6, context->m_internal.texture__loadedTexture_6_format, DX12Utils::AccessType::SRV, DX12Utils::ResourceType::Texture2DArray, false, 0, context->m_internal.texture__loadedTexture_6_size[2], 0 },
                { context->m_internal.texture__loadedTexture_7, context->m_internal.texture__loadedTexture_7_format, DX12Utils::AccessType::SRV, DX12Utils::ResourceType::Texture2DArray, false, 0, context->m_internal.texture__loadedTexture_7_size[2], 0 },
                { context->m_internal.texture__loadedTexture_8, context->m_internal.texture__loadedTexture_8_format, DX12Utils::AccessType::SRV, DX12Utils::ResourceType::Texture2D, false, 0, 0, 0 },
                { context->m_internal.texture__loadedTexture_9, context->m_internal.texture__loadedTexture_9_format, DX12Utils::AccessType::SRV, DX12Utils::ResourceType::Texture2D, false, 0, 0, 0 },
                { context->m_internal.constantBuffer__ThresholdCB, DXGI_FORMAT_UNKNOWN, DX12Utils::AccessType::CBV, DX12Utils::ResourceType::Buffer, false, 256, 1, 0 }
            };

            D3D12_GPU_DESCRIPTOR_HANDLE descriptorTable = GetDescriptorTable(device, s_srvHeap, descriptors, 14, Context::LogFn);
            commandList->SetComputeRootDescriptorTable(0, descriptorTable);

            unsigned int baseDispatchSize[3] = {
                context->m_output.texture_Color_size[0],
                context->m_output.texture_Color_size[1],
                context->m_output.texture_Color_size[2]
            };

            unsigned int dispatchSize[3] = {
                (((baseDispatchSize[0] + 0) * 1) / 1 + 0 + 8 - 1) / 8,
                (((baseDispatchSize[1] + 0) * 1) / 1 + 0 + 8 - 1) / 8,
                (((baseDispatchSize[2] + 0) * 1) / 1 + 0 + 1 - 1) / 1
            };

            commandList->Dispatch(dispatchSize[0], dispatchSize[1], dispatchSize[2]);

            if(context->m_profile)
            {
                context->m_profileData[(s_timerIndex-1)/2].m_label = "Threshold";
                context->m_profileData[(s_timerIndex-1)/2].m_cpu = (float)std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - startPointCPU).count();
                commandList->EndQuery(context->m_internal.m_TimestampQueryHeap, D3D12_QUERY_TYPE_TIMESTAMP, s_timerIndex++);
            }
        }

        // Shader Constants: _SpatialFilterCB
        {
            context->m_internal.constantBuffer__SpatialFilterCB_cpu.SpatialFilter1 = (int)context->m_input.variable_SpatialFilter1;
            context->m_internal.constantBuffer__SpatialFilterCB_cpu.SpatialFilter2 = (int)context->m_input.variable_SpatialFilter2;
            context->m_internal.constantBuffer__SpatialFilterCB_cpu.SpatialFilter3 = (int)context->m_input.variable_SpatialFilter3;
            context->m_internal.constantBuffer__SpatialFilterCB_cpu.SpatialFilter4 = (int)context->m_input.variable_SpatialFilter4;
            context->m_internal.constantBuffer__SpatialFilterCB_cpu.SpatialFilterParam1 = context->m_input.variable_SpatialFilterParam1;
            context->m_internal.constantBuffer__SpatialFilterCB_cpu.SpatialFilterParam2 = context->m_input.variable_SpatialFilterParam2;
            context->m_internal.constantBuffer__SpatialFilterCB_cpu.SpatialFilterParam3 = context->m_input.variable_SpatialFilterParam3;
            context->m_internal.constantBuffer__SpatialFilterCB_cpu.SpatialFilterParam4 = context->m_input.variable_SpatialFilterParam4;
            DX12Utils::CopyConstantsCPUToGPU(s_ubTracker, device, commandList, context->m_internal.constantBuffer__SpatialFilterCB, context->m_internal.constantBuffer__SpatialFilterCB_cpu, Context::LogFn);
        }

        // Transition resources for the next action
        {
            D3D12_RESOURCE_BARRIER barriers[2];

            barriers[0].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            barriers[0].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            barriers[0].Transition.pResource = context->m_output.texture_Color;
            barriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
            barriers[0].Transition.StateAfter = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
            barriers[0].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

            barriers[1].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            barriers[1].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            barriers[1].Transition.pResource = context->m_internal.texture_Temp;
            barriers[1].Transition.StateBefore = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
            barriers[1].Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
            barriers[1].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

            commandList->ResourceBarrier(2, barriers);
        }

        // Compute Shader: SpatialFilter
        {
            ScopedPerfEvent scopedPerf("Compute Shader: SpatialFilter", commandList, 4);
            std::chrono::high_resolution_clock::time_point startPointCPU;
            if(context->m_profile)
            {
                startPointCPU = std::chrono::high_resolution_clock::now();
                commandList->EndQuery(context->m_internal.m_TimestampQueryHeap, D3D12_QUERY_TYPE_TIMESTAMP, s_timerIndex++);
            }

            commandList->SetComputeRootSignature(ContextInternal::computeShader_SpatialFilter_rootSig);
            commandList->SetPipelineState(ContextInternal::computeShader_SpatialFilter_pso);

            DX12Utils::ResourceDescriptor descriptors[] = {
                { context->m_output.texture_Color, context->m_output.texture_Color_format, DX12Utils::AccessType::SRV, DX12Utils::ResourceType::Texture2D, false, 0, 0, 0 },
                { context->m_internal.texture_Temp, context->m_internal.texture_Temp_format, DX12Utils::AccessType::UAV, DX12Utils::ResourceType::Texture2D, false, 0, 0, 0 },
                { context->m_internal.constantBuffer__SpatialFilterCB, DXGI_FORMAT_UNKNOWN, DX12Utils::AccessType::CBV, DX12Utils::ResourceType::Buffer, false, 256, 1, 0 }
            };

            D3D12_GPU_DESCRIPTOR_HANDLE descriptorTable = GetDescriptorTable(device, s_srvHeap, descriptors, 3, Context::LogFn);
            commandList->SetComputeRootDescriptorTable(0, descriptorTable);

            unsigned int baseDispatchSize[3] = {
                context->m_output.texture_Color_size[0],
                context->m_output.texture_Color_size[1],
                context->m_output.texture_Color_size[2]
            };

            unsigned int dispatchSize[3] = {
                (((baseDispatchSize[0] + 0) * 1) / 1 + 0 + 8 - 1) / 8,
                (((baseDispatchSize[1] + 0) * 1) / 1 + 0 + 8 - 1) / 8,
                (((baseDispatchSize[2] + 0) * 1) / 1 + 0 + 1 - 1) / 1
            };

            commandList->Dispatch(dispatchSize[0], dispatchSize[1], dispatchSize[2]);

            if(context->m_profile)
            {
                context->m_profileData[(s_timerIndex-1)/2].m_label = "SpatialFilter";
                context->m_profileData[(s_timerIndex-1)/2].m_cpu = (float)std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - startPointCPU).count();
                commandList->EndQuery(context->m_internal.m_TimestampQueryHeap, D3D12_QUERY_TYPE_TIMESTAMP, s_timerIndex++);
            }
        }

        // Shader Constants: _TemporalFilterCB
        {
            context->m_internal.constantBuffer__TemporalFilterCB_cpu.NeighborhoodClamp1 = context->m_input.variable_NeighborhoodClamp1;
            context->m_internal.constantBuffer__TemporalFilterCB_cpu.NeighborhoodClamp2 = context->m_input.variable_NeighborhoodClamp2;
            context->m_internal.constantBuffer__TemporalFilterCB_cpu.NeighborhoodClamp3 = context->m_input.variable_NeighborhoodClamp3;
            context->m_internal.constantBuffer__TemporalFilterCB_cpu.NeighborhoodClamp4 = context->m_input.variable_NeighborhoodClamp4;
            context->m_internal.constantBuffer__TemporalFilterCB_cpu.TemporalFilterAlpha1 = context->m_input.variable_TemporalFilterAlpha1;
            context->m_internal.constantBuffer__TemporalFilterCB_cpu.TemporalFilterAlpha2 = context->m_input.variable_TemporalFilterAlpha2;
            context->m_internal.constantBuffer__TemporalFilterCB_cpu.TemporalFilterAlpha3 = context->m_input.variable_TemporalFilterAlpha3;
            context->m_internal.constantBuffer__TemporalFilterCB_cpu.TemporalFilterAlpha4 = context->m_input.variable_TemporalFilterAlpha4;
            DX12Utils::CopyConstantsCPUToGPU(s_ubTracker, device, commandList, context->m_internal.constantBuffer__TemporalFilterCB, context->m_internal.constantBuffer__TemporalFilterCB_cpu, Context::LogFn);
        }

        // Transition resources for the next action
        {
            D3D12_RESOURCE_BARRIER barriers[3];

            barriers[0].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            barriers[0].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            barriers[0].Transition.pResource = context->m_output.texture_Color;
            barriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
            barriers[0].Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
            barriers[0].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

            barriers[1].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            barriers[1].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            barriers[1].Transition.pResource = context->m_internal.texture_Temp;
            barriers[1].Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
            barriers[1].Transition.StateAfter = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
            barriers[1].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

            barriers[2].Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
            barriers[2].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            barriers[2].UAV.pResource = context->m_internal.texture_TemporalAccum;

            commandList->ResourceBarrier(3, barriers);
        }

        // Compute Shader: TemporalFilter
        {
            ScopedPerfEvent scopedPerf("Compute Shader: TemporalFilter", commandList, 6);
            std::chrono::high_resolution_clock::time_point startPointCPU;
            if(context->m_profile)
            {
                startPointCPU = std::chrono::high_resolution_clock::now();
                commandList->EndQuery(context->m_internal.m_TimestampQueryHeap, D3D12_QUERY_TYPE_TIMESTAMP, s_timerIndex++);
            }

            commandList->SetComputeRootSignature(ContextInternal::computeShader_TemporalFilter_rootSig);
            commandList->SetPipelineState(ContextInternal::computeShader_TemporalFilter_pso);

            DX12Utils::ResourceDescriptor descriptors[] = {
                { context->m_internal.texture_Temp, context->m_internal.texture_Temp_format, DX12Utils::AccessType::SRV, DX12Utils::ResourceType::Texture2D, false, 0, 0, 0 },
                { context->m_output.texture_Color, context->m_output.texture_Color_format, DX12Utils::AccessType::UAV, DX12Utils::ResourceType::Texture2D, false, 0, 0, 0 },
                { context->m_internal.texture_TemporalAccum, context->m_internal.texture_TemporalAccum_format, DX12Utils::AccessType::UAV, DX12Utils::ResourceType::Texture2D, false, 0, 0, 0 },
                { context->m_internal.constantBuffer__TemporalFilterCB, DXGI_FORMAT_UNKNOWN, DX12Utils::AccessType::CBV, DX12Utils::ResourceType::Buffer, false, 256, 1, 0 }
            };

            D3D12_GPU_DESCRIPTOR_HANDLE descriptorTable = GetDescriptorTable(device, s_srvHeap, descriptors, 4, Context::LogFn);
            commandList->SetComputeRootDescriptorTable(0, descriptorTable);

            unsigned int baseDispatchSize[3] = {
                context->m_output.texture_Color_size[0],
                context->m_output.texture_Color_size[1],
                context->m_output.texture_Color_size[2]
            };

            unsigned int dispatchSize[3] = {
                (((baseDispatchSize[0] + 0) * 1) / 1 + 0 + 8 - 1) / 8,
                (((baseDispatchSize[1] + 0) * 1) / 1 + 0 + 8 - 1) / 8,
                (((baseDispatchSize[2] + 0) * 1) / 1 + 0 + 1 - 1) / 1
            };

            commandList->Dispatch(dispatchSize[0], dispatchSize[1], dispatchSize[2]);

            if(context->m_profile)
            {
                context->m_profileData[(s_timerIndex-1)/2].m_label = "TemporalFilter";
                context->m_profileData[(s_timerIndex-1)/2].m_cpu = (float)std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - startPointCPU).count();
                commandList->EndQuery(context->m_internal.m_TimestampQueryHeap, D3D12_QUERY_TYPE_TIMESTAMP, s_timerIndex++);
            }
        }

        // Shader Constants: _AdjustBrightnessCB
        {
            context->m_internal.constantBuffer__AdjustBrightnessCB_cpu.BrightnessMultiplier = context->m_input.variable_BrightnessMultiplier;
            DX12Utils::CopyConstantsCPUToGPU(s_ubTracker, device, commandList, context->m_internal.constantBuffer__AdjustBrightnessCB, context->m_internal.constantBuffer__AdjustBrightnessCB_cpu, Context::LogFn);
        }

        // Transition resources for the next action
        {
            D3D12_RESOURCE_BARRIER barriers[1];

            barriers[0].Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
            barriers[0].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            barriers[0].UAV.pResource = context->m_output.texture_Color;

            commandList->ResourceBarrier(1, barriers);
        }

        // Compute Shader: AdjustBrightness
        {
            ScopedPerfEvent scopedPerf("Compute Shader: AdjustBrightness", commandList, 7);
            std::chrono::high_resolution_clock::time_point startPointCPU;
            if(context->m_profile)
            {
                startPointCPU = std::chrono::high_resolution_clock::now();
                commandList->EndQuery(context->m_internal.m_TimestampQueryHeap, D3D12_QUERY_TYPE_TIMESTAMP, s_timerIndex++);
            }

            commandList->SetComputeRootSignature(ContextInternal::computeShader_AdjustBrightness_rootSig);
            commandList->SetPipelineState(ContextInternal::computeShader_AdjustBrightness_pso);

            DX12Utils::ResourceDescriptor descriptors[] = {
                { context->m_output.texture_Color, context->m_output.texture_Color_format, DX12Utils::AccessType::UAV, DX12Utils::ResourceType::Texture2D, false, 0, 0, 0 },
                { context->m_internal.constantBuffer__AdjustBrightnessCB, DXGI_FORMAT_UNKNOWN, DX12Utils::AccessType::CBV, DX12Utils::ResourceType::Buffer, false, 256, 1, 0 }
            };

            D3D12_GPU_DESCRIPTOR_HANDLE descriptorTable = GetDescriptorTable(device, s_srvHeap, descriptors, 2, Context::LogFn);
            commandList->SetComputeRootDescriptorTable(0, descriptorTable);

            unsigned int baseDispatchSize[3] = {
                context->m_output.texture_Color_size[0],
                context->m_output.texture_Color_size[1],
                context->m_output.texture_Color_size[2]
            };

            unsigned int dispatchSize[3] = {
                (((baseDispatchSize[0] + 0) * 1) / 1 + 0 + 8 - 1) / 8,
                (((baseDispatchSize[1] + 0) * 1) / 1 + 0 + 8 - 1) / 8,
                (((baseDispatchSize[2] + 0) * 1) / 1 + 0 + 1 - 1) / 1
            };

            commandList->Dispatch(dispatchSize[0], dispatchSize[1], dispatchSize[2]);

            if(context->m_profile)
            {
                context->m_profileData[(s_timerIndex-1)/2].m_label = "AdjustBrightness";
                context->m_profileData[(s_timerIndex-1)/2].m_cpu = (float)std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - startPointCPU).count();
                commandList->EndQuery(context->m_internal.m_TimestampQueryHeap, D3D12_QUERY_TYPE_TIMESTAMP, s_timerIndex++);
            }
        }

        // Make sure imported resources are put back in the state they were given to us in
        {
            int barrierCount = 0;
            D3D12_RESOURCE_BARRIER barriers[2];

            if(context->m_input.texture_Input_state != D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE)
            {
                barriers[barrierCount].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                barriers[barrierCount].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                barriers[barrierCount].Transition.pResource = context->m_input.texture_Input;
                barriers[barrierCount].Transition.StateBefore = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
                barriers[barrierCount].Transition.StateAfter = context->m_input.texture_Input_state;
                barriers[barrierCount].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
                barrierCount++;
            }

            if(context->m_input.texture_ThresholdMap_state != D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE)
            {
                barriers[barrierCount].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                barriers[barrierCount].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                barriers[barrierCount].Transition.pResource = context->m_input.texture_ThresholdMap;
                barriers[barrierCount].Transition.StateBefore = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
                barriers[barrierCount].Transition.StateAfter = context->m_input.texture_ThresholdMap_state;
                barriers[barrierCount].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
                barrierCount++;
            }

            if(barrierCount > 0)
                commandList->ResourceBarrier(barrierCount, barriers);
        }

        if(context->m_profile)
        {
            context->m_profileData[(s_timerIndex-1)/2].m_label = "Total";
            context->m_profileData[(s_timerIndex-1)/2].m_cpu = (float)std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - startPointCPUTechnique).count();
            commandList->EndQuery(context->m_internal.m_TimestampQueryHeap, D3D12_QUERY_TYPE_TIMESTAMP, s_timerIndex++);
            commandList->ResolveQueryData(context->m_internal.m_TimestampQueryHeap, D3D12_QUERY_TYPE_TIMESTAMP, 0, s_timerIndex, context->m_internal.m_TimestampReadbackBuffer, 0);
        }
    }

    void Context::EnsureResourcesCreated(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
    {
        bool dirty = false;

        // Color
        {
            unsigned int baseSize[3] = {
                m_input.texture_Input_size[0],
                m_input.texture_Input_size[1],
                m_input.texture_Input_size[2]
            };

            unsigned int desiredSize[3] = {
                ((baseSize[0] + 0) * 2) / 1 + 0,
                ((baseSize[1] + 0) * 2) / 1 + 0,
                ((baseSize[2] + 0) * 1) / 1 + 0
            };

            static const unsigned int desiredNumMips = 1;

            DXGI_FORMAT desiredFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

            if(!m_output.texture_Color ||
               m_output.texture_Color_size[0] != desiredSize[0] ||
               m_output.texture_Color_size[1] != desiredSize[1] ||
               m_output.texture_Color_size[2] != desiredSize[2] ||
               m_output.texture_Color_numMips != desiredNumMips ||
               m_output.texture_Color_format != desiredFormat)
            {
                dirty = true;
                if(m_output.texture_Color)
                    s_delayedRelease.Add(m_output.texture_Color);

                m_output.texture_Color = DX12Utils::CreateTexture(device, desiredSize, desiredNumMips, desiredFormat, m_output.texture_Color_flags, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, DX12Utils::ResourceType::Texture2D, (c_debugNames ? L"Color" : nullptr), Context::LogFn);
                m_output.texture_Color_size[0] = desiredSize[0];
                m_output.texture_Color_size[1] = desiredSize[1];
                m_output.texture_Color_size[2] = desiredSize[2];
                m_output.texture_Color_numMips = desiredNumMips;
                m_output.texture_Color_format = desiredFormat;
            }
        }

        // Temp
        {
            unsigned int baseSize[3] = {
                m_output.texture_Color_size[0],
                m_output.texture_Color_size[1],
                m_output.texture_Color_size[2]
            };

            unsigned int desiredSize[3] = {
                ((baseSize[0] + 0) * 1) / 1 + 0,
                ((baseSize[1] + 0) * 1) / 1 + 0,
                ((baseSize[2] + 0) * 1) / 1 + 0
            };

            static const unsigned int desiredNumMips = 1;

            DXGI_FORMAT desiredFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

            if(!m_internal.texture_Temp ||
               m_internal.texture_Temp_size[0] != desiredSize[0] ||
               m_internal.texture_Temp_size[1] != desiredSize[1] ||
               m_internal.texture_Temp_size[2] != desiredSize[2] ||
               m_internal.texture_Temp_numMips != desiredNumMips ||
               m_internal.texture_Temp_format != desiredFormat)
            {
                dirty = true;
                if(m_internal.texture_Temp)
                    s_delayedRelease.Add(m_internal.texture_Temp);

                m_internal.texture_Temp = DX12Utils::CreateTexture(device, desiredSize, desiredNumMips, desiredFormat, m_internal.texture_Temp_flags, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, DX12Utils::ResourceType::Texture2D, (c_debugNames ? L"Temp" : nullptr), Context::LogFn);
                m_internal.texture_Temp_size[0] = desiredSize[0];
                m_internal.texture_Temp_size[1] = desiredSize[1];
                m_internal.texture_Temp_size[2] = desiredSize[2];
                m_internal.texture_Temp_numMips = desiredNumMips;
                m_internal.texture_Temp_format = desiredFormat;
            }
        }

        // TemporalAccum
        {
            unsigned int baseSize[3] = {
                m_output.texture_Color_size[0],
                m_output.texture_Color_size[1],
                m_output.texture_Color_size[2]
            };

            unsigned int desiredSize[3] = {
                ((baseSize[0] + 0) * 1) / 1 + 0,
                ((baseSize[1] + 0) * 1) / 1 + 0,
                ((baseSize[2] + 0) * 1) / 1 + 0
            };

            static const unsigned int desiredNumMips = 1;

            DXGI_FORMAT desiredFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;

            if(!m_internal.texture_TemporalAccum ||
               m_internal.texture_TemporalAccum_size[0] != desiredSize[0] ||
               m_internal.texture_TemporalAccum_size[1] != desiredSize[1] ||
               m_internal.texture_TemporalAccum_size[2] != desiredSize[2] ||
               m_internal.texture_TemporalAccum_numMips != desiredNumMips ||
               m_internal.texture_TemporalAccum_format != desiredFormat)
            {
                dirty = true;
                if(m_internal.texture_TemporalAccum)
                    s_delayedRelease.Add(m_internal.texture_TemporalAccum);

                m_internal.texture_TemporalAccum = DX12Utils::CreateTexture(device, desiredSize, desiredNumMips, desiredFormat, m_internal.texture_TemporalAccum_flags, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, DX12Utils::ResourceType::Texture2D, (c_debugNames ? L"TemporalAccum" : nullptr), Context::LogFn);
                m_internal.texture_TemporalAccum_size[0] = desiredSize[0];
                m_internal.texture_TemporalAccum_size[1] = desiredSize[1];
                m_internal.texture_TemporalAccum_size[2] = desiredSize[2];
                m_internal.texture_TemporalAccum_numMips = desiredNumMips;
                m_internal.texture_TemporalAccum_format = desiredFormat;
            }
        }

        // _loadedTexture_0
        {
            if (!m_internal.texture__loadedTexture_0)
            {
                // Load the texture
                std::vector<DX12Utils::TextureCache::Texture> loadedTextureSlices;
                DX12Utils::DXGI_FORMAT_Info formatInfo = DX12Utils::Get_DXGI_FORMAT_Info(DXGI_FORMAT_R8_UNORM, Context::LogFn);
                DX12Utils::TextureCache::Type desiredType = DX12Utils::TextureCache::Type::U8;
                if (formatInfo.channelType == DX12Utils::DXGI_FORMAT_Info::ChannelType::_uint8_t)
                    desiredType = DX12Utils::TextureCache::Type::U8;
                else if (formatInfo.channelType == DX12Utils::DXGI_FORMAT_Info::ChannelType::_float)
                    desiredType = DX12Utils::TextureCache::Type::F32;
                else
                    Context::LogFn(LogLevel::Error, "Unhandled channel type for image: Textures/FAST_Blue2D/blue2d_0.png");

                char loadedTextureFileName[1024];
                sprintf_s(loadedTextureFileName, "%lsassets/Textures/FAST_Blue2D/blue2d_0.png", s_techniqueLocation.c_str());

                loadedTextureSlices.push_back(DX12Utils::TextureCache::GetAs(loadedTextureFileName, false, desiredType, formatInfo.sRGB, formatInfo.channelCount));
                DX12Utils::TextureCache::Texture& loadedTexture = loadedTextureSlices[0];
                if(!loadedTexture.Valid())
                    Context::LogFn(LogLevel::Error, "Could not load image: Textures/FAST_Blue2D/blue2d_0.png");

                unsigned int size[3] = { (unsigned int)loadedTexture.width, (unsigned int)loadedTexture.height, 1 };

                static const unsigned int desiredNumMips = 1;

                // Create the texture
                dirty = true;
                m_internal.texture__loadedTexture_0_size[0] = size[0];
                m_internal.texture__loadedTexture_0_size[1] = size[1];
                m_internal.texture__loadedTexture_0_size[2] = size[2];
                m_internal.texture__loadedTexture_0_numMips = desiredNumMips;
                m_internal.texture__loadedTexture_0_format = DXGI_FORMAT_R8_UNORM;
                m_internal.texture__loadedTexture_0 = DX12Utils::CreateTexture(device, size, desiredNumMips, DXGI_FORMAT_R8_UNORM, m_internal.texture__loadedTexture_0_flags, D3D12_RESOURCE_STATE_COPY_DEST, DX12Utils::ResourceType::Texture2D, (c_debugNames ? L"_loadedTexture_0" : nullptr), Context::LogFn);


                std::vector<unsigned char> pixels;
                for (const DX12Utils::TextureCache::Texture& texture : loadedTextureSlices)
                    pixels.insert(pixels.end(), texture.pixels.begin(), texture.pixels.end());

                DX12Utils::UploadTextureToGPUAndMakeMips(device, commandList, s_ubTracker, m_internal.texture__loadedTexture_0, pixels, size, desiredNumMips, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, LogFn);
            }
        }

        // _loadedTexture_1
        {
            if (!m_internal.texture__loadedTexture_1)
            {
                // Load the texture
                std::vector<DX12Utils::TextureCache::Texture> loadedTextureSlices;
                DX12Utils::DXGI_FORMAT_Info formatInfo = DX12Utils::Get_DXGI_FORMAT_Info(DXGI_FORMAT_R8_UNORM, Context::LogFn);
                DX12Utils::TextureCache::Type desiredType = DX12Utils::TextureCache::Type::U8;
                if (formatInfo.channelType == DX12Utils::DXGI_FORMAT_Info::ChannelType::_uint8_t)
                    desiredType = DX12Utils::TextureCache::Type::U8;
                else if (formatInfo.channelType == DX12Utils::DXGI_FORMAT_Info::ChannelType::_float)
                    desiredType = DX12Utils::TextureCache::Type::F32;
                else
                    Context::LogFn(LogLevel::Error, "Unhandled channel type");

                int textureIndex = -1;
                while(1)
                {
                    textureIndex++;
                    char indexedFileName[1024];
                    sprintf_s(indexedFileName, "%lsassets/Textures/FAST_Blue2D/blue2d_%i.png", s_techniqueLocation.c_str(), textureIndex);
                    DX12Utils::TextureCache::Texture loadedTextureSlice = DX12Utils::TextureCache::GetAs(indexedFileName, false, desiredType, formatInfo.sRGB, formatInfo.channelCount);

                    if(!loadedTextureSlice.Valid())
                    {
                        if (textureIndex == 0)
                            Context::LogFn(LogLevel::Error, "Could not load image: %s", indexedFileName);
                        break;
                    }

                    if (textureIndex > 0 && (loadedTextureSlice.width != loadedTextureSlices[0].width || loadedTextureSlice.height != loadedTextureSlices[0].height))
                        Context::LogFn(LogLevel::Error, "%s does not match dimensions of the first texture loaded!", indexedFileName);

                    loadedTextureSlices.push_back(loadedTextureSlice);
                }

                unsigned int size[3] = { (unsigned int)loadedTextureSlices[0].width, (unsigned int)loadedTextureSlices[0].height, (unsigned int)loadedTextureSlices.size() };

                static const unsigned int desiredNumMips = 1;

                // Create the texture
                dirty = true;
                m_internal.texture__loadedTexture_1_size[0] = size[0];
                m_internal.texture__loadedTexture_1_size[1] = size[1];
                m_internal.texture__loadedTexture_1_size[2] = size[2];
                m_internal.texture__loadedTexture_1_numMips = desiredNumMips;
                m_internal.texture__loadedTexture_1_format = DXGI_FORMAT_R8_UNORM;
                m_internal.texture__loadedTexture_1 = DX12Utils::CreateTexture(device, size, desiredNumMips, DXGI_FORMAT_R8_UNORM, m_internal.texture__loadedTexture_1_flags, D3D12_RESOURCE_STATE_COPY_DEST, DX12Utils::ResourceType::Texture2DArray, (c_debugNames ? L"_loadedTexture_1" : nullptr), Context::LogFn);


                std::vector<unsigned char> pixels;
                for (const DX12Utils::TextureCache::Texture& texture : loadedTextureSlices)
                    pixels.insert(pixels.end(), texture.pixels.begin(), texture.pixels.end());

                DX12Utils::UploadTextureToGPUAndMakeMips(device, commandList, s_ubTracker, m_internal.texture__loadedTexture_1, pixels, size, desiredNumMips, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, LogFn);
            }
        }

        // _loadedTexture_2
        {
            if (!m_internal.texture__loadedTexture_2)
            {
                // Load the texture
                std::vector<DX12Utils::TextureCache::Texture> loadedTextureSlices;
                DX12Utils::DXGI_FORMAT_Info formatInfo = DX12Utils::Get_DXGI_FORMAT_Info(DXGI_FORMAT_R8_UNORM, Context::LogFn);
                DX12Utils::TextureCache::Type desiredType = DX12Utils::TextureCache::Type::U8;
                if (formatInfo.channelType == DX12Utils::DXGI_FORMAT_Info::ChannelType::_uint8_t)
                    desiredType = DX12Utils::TextureCache::Type::U8;
                else if (formatInfo.channelType == DX12Utils::DXGI_FORMAT_Info::ChannelType::_float)
                    desiredType = DX12Utils::TextureCache::Type::F32;
                else
                    Context::LogFn(LogLevel::Error, "Unhandled channel type");

                int textureIndex = -1;
                while(1)
                {
                    textureIndex++;
                    char indexedFileName[1024];
                    sprintf_s(indexedFileName, "%lsassets/Textures/STBN_10/stbn_scalar_10_2Dx1Dx1D_128x128x32x1_%i.png", s_techniqueLocation.c_str(), textureIndex);
                    DX12Utils::TextureCache::Texture loadedTextureSlice = DX12Utils::TextureCache::GetAs(indexedFileName, false, desiredType, formatInfo.sRGB, formatInfo.channelCount);

                    if(!loadedTextureSlice.Valid())
                    {
                        if (textureIndex == 0)
                            Context::LogFn(LogLevel::Error, "Could not load image: %s", indexedFileName);
                        break;
                    }

                    if (textureIndex > 0 && (loadedTextureSlice.width != loadedTextureSlices[0].width || loadedTextureSlice.height != loadedTextureSlices[0].height))
                        Context::LogFn(LogLevel::Error, "%s does not match dimensions of the first texture loaded!", indexedFileName);

                    loadedTextureSlices.push_back(loadedTextureSlice);
                }

                unsigned int size[3] = { (unsigned int)loadedTextureSlices[0].width, (unsigned int)loadedTextureSlices[0].height, (unsigned int)loadedTextureSlices.size() };

                static const unsigned int desiredNumMips = 1;

                // Create the texture
                dirty = true;
                m_internal.texture__loadedTexture_2_size[0] = size[0];
                m_internal.texture__loadedTexture_2_size[1] = size[1];
                m_internal.texture__loadedTexture_2_size[2] = size[2];
                m_internal.texture__loadedTexture_2_numMips = desiredNumMips;
                m_internal.texture__loadedTexture_2_format = DXGI_FORMAT_R8_UNORM;
                m_internal.texture__loadedTexture_2 = DX12Utils::CreateTexture(device, size, desiredNumMips, DXGI_FORMAT_R8_UNORM, m_internal.texture__loadedTexture_2_flags, D3D12_RESOURCE_STATE_COPY_DEST, DX12Utils::ResourceType::Texture2DArray, (c_debugNames ? L"_loadedTexture_2" : nullptr), Context::LogFn);


                std::vector<unsigned char> pixels;
                for (const DX12Utils::TextureCache::Texture& texture : loadedTextureSlices)
                    pixels.insert(pixels.end(), texture.pixels.begin(), texture.pixels.end());

                DX12Utils::UploadTextureToGPUAndMakeMips(device, commandList, s_ubTracker, m_internal.texture__loadedTexture_2, pixels, size, desiredNumMips, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, LogFn);
            }
        }

        // _loadedTexture_3
        {
            if (!m_internal.texture__loadedTexture_3)
            {
                // Load the texture
                std::vector<DX12Utils::TextureCache::Texture> loadedTextureSlices;
                DX12Utils::DXGI_FORMAT_Info formatInfo = DX12Utils::Get_DXGI_FORMAT_Info(DXGI_FORMAT_R8_UNORM, Context::LogFn);
                DX12Utils::TextureCache::Type desiredType = DX12Utils::TextureCache::Type::U8;
                if (formatInfo.channelType == DX12Utils::DXGI_FORMAT_Info::ChannelType::_uint8_t)
                    desiredType = DX12Utils::TextureCache::Type::U8;
                else if (formatInfo.channelType == DX12Utils::DXGI_FORMAT_Info::ChannelType::_float)
                    desiredType = DX12Utils::TextureCache::Type::F32;
                else
                    Context::LogFn(LogLevel::Error, "Unhandled channel type");

                int textureIndex = -1;
                while(1)
                {
                    textureIndex++;
                    char indexedFileName[1024];
                    sprintf_s(indexedFileName, "%lsassets/Textures/STBN_19/stbn_scalar_19_2Dx1Dx1D_128x128x32x1_%i.png", s_techniqueLocation.c_str(), textureIndex);
                    DX12Utils::TextureCache::Texture loadedTextureSlice = DX12Utils::TextureCache::GetAs(indexedFileName, false, desiredType, formatInfo.sRGB, formatInfo.channelCount);

                    if(!loadedTextureSlice.Valid())
                    {
                        if (textureIndex == 0)
                            Context::LogFn(LogLevel::Error, "Could not load image: %s", indexedFileName);
                        break;
                    }

                    if (textureIndex > 0 && (loadedTextureSlice.width != loadedTextureSlices[0].width || loadedTextureSlice.height != loadedTextureSlices[0].height))
                        Context::LogFn(LogLevel::Error, "%s does not match dimensions of the first texture loaded!", indexedFileName);

                    loadedTextureSlices.push_back(loadedTextureSlice);
                }

                unsigned int size[3] = { (unsigned int)loadedTextureSlices[0].width, (unsigned int)loadedTextureSlices[0].height, (unsigned int)loadedTextureSlices.size() };

                static const unsigned int desiredNumMips = 1;

                // Create the texture
                dirty = true;
                m_internal.texture__loadedTexture_3_size[0] = size[0];
                m_internal.texture__loadedTexture_3_size[1] = size[1];
                m_internal.texture__loadedTexture_3_size[2] = size[2];
                m_internal.texture__loadedTexture_3_numMips = desiredNumMips;
                m_internal.texture__loadedTexture_3_format = DXGI_FORMAT_R8_UNORM;
                m_internal.texture__loadedTexture_3 = DX12Utils::CreateTexture(device, size, desiredNumMips, DXGI_FORMAT_R8_UNORM, m_internal.texture__loadedTexture_3_flags, D3D12_RESOURCE_STATE_COPY_DEST, DX12Utils::ResourceType::Texture2DArray, (c_debugNames ? L"_loadedTexture_3" : nullptr), Context::LogFn);


                std::vector<unsigned char> pixels;
                for (const DX12Utils::TextureCache::Texture& texture : loadedTextureSlices)
                    pixels.insert(pixels.end(), texture.pixels.begin(), texture.pixels.end());

                DX12Utils::UploadTextureToGPUAndMakeMips(device, commandList, s_ubTracker, m_internal.texture__loadedTexture_3, pixels, size, desiredNumMips, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, LogFn);
            }
        }

        // _loadedTexture_4
        {
            if (!m_internal.texture__loadedTexture_4)
            {
                // Load the texture
                std::vector<DX12Utils::TextureCache::Texture> loadedTextureSlices;
                DX12Utils::DXGI_FORMAT_Info formatInfo = DX12Utils::Get_DXGI_FORMAT_Info(DXGI_FORMAT_R8_UNORM, Context::LogFn);
                DX12Utils::TextureCache::Type desiredType = DX12Utils::TextureCache::Type::U8;
                if (formatInfo.channelType == DX12Utils::DXGI_FORMAT_Info::ChannelType::_uint8_t)
                    desiredType = DX12Utils::TextureCache::Type::U8;
                else if (formatInfo.channelType == DX12Utils::DXGI_FORMAT_Info::ChannelType::_float)
                    desiredType = DX12Utils::TextureCache::Type::F32;
                else
                    Context::LogFn(LogLevel::Error, "Unhandled channel type");

                int textureIndex = -1;
                while(1)
                {
                    textureIndex++;
                    char indexedFileName[1024];
                    sprintf_s(indexedFileName, "%lsassets/Textures/FAST_Blue_Exp/real_uniform_gauss1_0_exp0101_separate05_%i.png", s_techniqueLocation.c_str(), textureIndex);
                    DX12Utils::TextureCache::Texture loadedTextureSlice = DX12Utils::TextureCache::GetAs(indexedFileName, false, desiredType, formatInfo.sRGB, formatInfo.channelCount);

                    if(!loadedTextureSlice.Valid())
                    {
                        if (textureIndex == 0)
                            Context::LogFn(LogLevel::Error, "Could not load image: %s", indexedFileName);
                        break;
                    }

                    if (textureIndex > 0 && (loadedTextureSlice.width != loadedTextureSlices[0].width || loadedTextureSlice.height != loadedTextureSlices[0].height))
                        Context::LogFn(LogLevel::Error, "%s does not match dimensions of the first texture loaded!", indexedFileName);

                    loadedTextureSlices.push_back(loadedTextureSlice);
                }

                unsigned int size[3] = { (unsigned int)loadedTextureSlices[0].width, (unsigned int)loadedTextureSlices[0].height, (unsigned int)loadedTextureSlices.size() };

                static const unsigned int desiredNumMips = 1;

                // Create the texture
                dirty = true;
                m_internal.texture__loadedTexture_4_size[0] = size[0];
                m_internal.texture__loadedTexture_4_size[1] = size[1];
                m_internal.texture__loadedTexture_4_size[2] = size[2];
                m_internal.texture__loadedTexture_4_numMips = desiredNumMips;
                m_internal.texture__loadedTexture_4_format = DXGI_FORMAT_R8_UNORM;
                m_internal.texture__loadedTexture_4 = DX12Utils::CreateTexture(device, size, desiredNumMips, DXGI_FORMAT_R8_UNORM, m_internal.texture__loadedTexture_4_flags, D3D12_RESOURCE_STATE_COPY_DEST, DX12Utils::ResourceType::Texture2DArray, (c_debugNames ? L"_loadedTexture_4" : nullptr), Context::LogFn);


                std::vector<unsigned char> pixels;
                for (const DX12Utils::TextureCache::Texture& texture : loadedTextureSlices)
                    pixels.insert(pixels.end(), texture.pixels.begin(), texture.pixels.end());

                DX12Utils::UploadTextureToGPUAndMakeMips(device, commandList, s_ubTracker, m_internal.texture__loadedTexture_4, pixels, size, desiredNumMips, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, LogFn);
            }
        }

        // _loadedTexture_5
        {
            if (!m_internal.texture__loadedTexture_5)
            {
                // Load the texture
                std::vector<DX12Utils::TextureCache::Texture> loadedTextureSlices;
                DX12Utils::DXGI_FORMAT_Info formatInfo = DX12Utils::Get_DXGI_FORMAT_Info(DXGI_FORMAT_R8_UNORM, Context::LogFn);
                DX12Utils::TextureCache::Type desiredType = DX12Utils::TextureCache::Type::U8;
                if (formatInfo.channelType == DX12Utils::DXGI_FORMAT_Info::ChannelType::_uint8_t)
                    desiredType = DX12Utils::TextureCache::Type::U8;
                else if (formatInfo.channelType == DX12Utils::DXGI_FORMAT_Info::ChannelType::_float)
                    desiredType = DX12Utils::TextureCache::Type::F32;
                else
                    Context::LogFn(LogLevel::Error, "Unhandled channel type");

                int textureIndex = -1;
                while(1)
                {
                    textureIndex++;
                    char indexedFileName[1024];
                    sprintf_s(indexedFileName, "%lsassets/Textures/FAST_Blue_Exp/real_uniform_gauss1_0_exp0101_product_%i.png", s_techniqueLocation.c_str(), textureIndex);
                    DX12Utils::TextureCache::Texture loadedTextureSlice = DX12Utils::TextureCache::GetAs(indexedFileName, false, desiredType, formatInfo.sRGB, formatInfo.channelCount);

                    if(!loadedTextureSlice.Valid())
                    {
                        if (textureIndex == 0)
                            Context::LogFn(LogLevel::Error, "Could not load image: %s", indexedFileName);
                        break;
                    }

                    if (textureIndex > 0 && (loadedTextureSlice.width != loadedTextureSlices[0].width || loadedTextureSlice.height != loadedTextureSlices[0].height))
                        Context::LogFn(LogLevel::Error, "%s does not match dimensions of the first texture loaded!", indexedFileName);

                    loadedTextureSlices.push_back(loadedTextureSlice);
                }

                unsigned int size[3] = { (unsigned int)loadedTextureSlices[0].width, (unsigned int)loadedTextureSlices[0].height, (unsigned int)loadedTextureSlices.size() };

                static const unsigned int desiredNumMips = 1;

                // Create the texture
                dirty = true;
                m_internal.texture__loadedTexture_5_size[0] = size[0];
                m_internal.texture__loadedTexture_5_size[1] = size[1];
                m_internal.texture__loadedTexture_5_size[2] = size[2];
                m_internal.texture__loadedTexture_5_numMips = desiredNumMips;
                m_internal.texture__loadedTexture_5_format = DXGI_FORMAT_R8_UNORM;
                m_internal.texture__loadedTexture_5 = DX12Utils::CreateTexture(device, size, desiredNumMips, DXGI_FORMAT_R8_UNORM, m_internal.texture__loadedTexture_5_flags, D3D12_RESOURCE_STATE_COPY_DEST, DX12Utils::ResourceType::Texture2DArray, (c_debugNames ? L"_loadedTexture_5" : nullptr), Context::LogFn);


                std::vector<unsigned char> pixels;
                for (const DX12Utils::TextureCache::Texture& texture : loadedTextureSlices)
                    pixels.insert(pixels.end(), texture.pixels.begin(), texture.pixels.end());

                DX12Utils::UploadTextureToGPUAndMakeMips(device, commandList, s_ubTracker, m_internal.texture__loadedTexture_5, pixels, size, desiredNumMips, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, LogFn);
            }
        }

        // _loadedTexture_6
        {
            if (!m_internal.texture__loadedTexture_6)
            {
                // Load the texture
                std::vector<DX12Utils::TextureCache::Texture> loadedTextureSlices;
                DX12Utils::DXGI_FORMAT_Info formatInfo = DX12Utils::Get_DXGI_FORMAT_Info(DXGI_FORMAT_R8_UNORM, Context::LogFn);
                DX12Utils::TextureCache::Type desiredType = DX12Utils::TextureCache::Type::U8;
                if (formatInfo.channelType == DX12Utils::DXGI_FORMAT_Info::ChannelType::_uint8_t)
                    desiredType = DX12Utils::TextureCache::Type::U8;
                else if (formatInfo.channelType == DX12Utils::DXGI_FORMAT_Info::ChannelType::_float)
                    desiredType = DX12Utils::TextureCache::Type::F32;
                else
                    Context::LogFn(LogLevel::Error, "Unhandled channel type");

                int textureIndex = -1;
                while(1)
                {
                    textureIndex++;
                    char indexedFileName[1024];
                    sprintf_s(indexedFileName, "%lsassets/Textures/FAST_Binomial3x3_Exp/real_uniform_binomial3x3_exp0101_product_%i.png", s_techniqueLocation.c_str(), textureIndex);
                    DX12Utils::TextureCache::Texture loadedTextureSlice = DX12Utils::TextureCache::GetAs(indexedFileName, false, desiredType, formatInfo.sRGB, formatInfo.channelCount);

                    if(!loadedTextureSlice.Valid())
                    {
                        if (textureIndex == 0)
                            Context::LogFn(LogLevel::Error, "Could not load image: %s", indexedFileName);
                        break;
                    }

                    if (textureIndex > 0 && (loadedTextureSlice.width != loadedTextureSlices[0].width || loadedTextureSlice.height != loadedTextureSlices[0].height))
                        Context::LogFn(LogLevel::Error, "%s does not match dimensions of the first texture loaded!", indexedFileName);

                    loadedTextureSlices.push_back(loadedTextureSlice);
                }

                unsigned int size[3] = { (unsigned int)loadedTextureSlices[0].width, (unsigned int)loadedTextureSlices[0].height, (unsigned int)loadedTextureSlices.size() };

                static const unsigned int desiredNumMips = 1;

                // Create the texture
                dirty = true;
                m_internal.texture__loadedTexture_6_size[0] = size[0];
                m_internal.texture__loadedTexture_6_size[1] = size[1];
                m_internal.texture__loadedTexture_6_size[2] = size[2];
                m_internal.texture__loadedTexture_6_numMips = desiredNumMips;
                m_internal.texture__loadedTexture_6_format = DXGI_FORMAT_R8_UNORM;
                m_internal.texture__loadedTexture_6 = DX12Utils::CreateTexture(device, size, desiredNumMips, DXGI_FORMAT_R8_UNORM, m_internal.texture__loadedTexture_6_flags, D3D12_RESOURCE_STATE_COPY_DEST, DX12Utils::ResourceType::Texture2DArray, (c_debugNames ? L"_loadedTexture_6" : nullptr), Context::LogFn);


                std::vector<unsigned char> pixels;
                for (const DX12Utils::TextureCache::Texture& texture : loadedTextureSlices)
                    pixels.insert(pixels.end(), texture.pixels.begin(), texture.pixels.end());

                DX12Utils::UploadTextureToGPUAndMakeMips(device, commandList, s_ubTracker, m_internal.texture__loadedTexture_6, pixels, size, desiredNumMips, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, LogFn);
            }
        }

        // _loadedTexture_7
        {
            if (!m_internal.texture__loadedTexture_7)
            {
                // Load the texture
                std::vector<DX12Utils::TextureCache::Texture> loadedTextureSlices;
                DX12Utils::DXGI_FORMAT_Info formatInfo = DX12Utils::Get_DXGI_FORMAT_Info(DXGI_FORMAT_R8_UNORM, Context::LogFn);
                DX12Utils::TextureCache::Type desiredType = DX12Utils::TextureCache::Type::U8;
                if (formatInfo.channelType == DX12Utils::DXGI_FORMAT_Info::ChannelType::_uint8_t)
                    desiredType = DX12Utils::TextureCache::Type::U8;
                else if (formatInfo.channelType == DX12Utils::DXGI_FORMAT_Info::ChannelType::_float)
                    desiredType = DX12Utils::TextureCache::Type::F32;
                else
                    Context::LogFn(LogLevel::Error, "Unhandled channel type");

                int textureIndex = -1;
                while(1)
                {
                    textureIndex++;
                    char indexedFileName[1024];
                    sprintf_s(indexedFileName, "%lsassets/Textures/FAST_Box3x3_Exp/real_uniform_box3x3_exp0101_product_%i.png", s_techniqueLocation.c_str(), textureIndex);
                    DX12Utils::TextureCache::Texture loadedTextureSlice = DX12Utils::TextureCache::GetAs(indexedFileName, false, desiredType, formatInfo.sRGB, formatInfo.channelCount);

                    if(!loadedTextureSlice.Valid())
                    {
                        if (textureIndex == 0)
                            Context::LogFn(LogLevel::Error, "Could not load image: %s", indexedFileName);
                        break;
                    }

                    if (textureIndex > 0 && (loadedTextureSlice.width != loadedTextureSlices[0].width || loadedTextureSlice.height != loadedTextureSlices[0].height))
                        Context::LogFn(LogLevel::Error, "%s does not match dimensions of the first texture loaded!", indexedFileName);

                    loadedTextureSlices.push_back(loadedTextureSlice);
                }

                unsigned int size[3] = { (unsigned int)loadedTextureSlices[0].width, (unsigned int)loadedTextureSlices[0].height, (unsigned int)loadedTextureSlices.size() };

                static const unsigned int desiredNumMips = 1;

                // Create the texture
                dirty = true;
                m_internal.texture__loadedTexture_7_size[0] = size[0];
                m_internal.texture__loadedTexture_7_size[1] = size[1];
                m_internal.texture__loadedTexture_7_size[2] = size[2];
                m_internal.texture__loadedTexture_7_numMips = desiredNumMips;
                m_internal.texture__loadedTexture_7_format = DXGI_FORMAT_R8_UNORM;
                m_internal.texture__loadedTexture_7 = DX12Utils::CreateTexture(device, size, desiredNumMips, DXGI_FORMAT_R8_UNORM, m_internal.texture__loadedTexture_7_flags, D3D12_RESOURCE_STATE_COPY_DEST, DX12Utils::ResourceType::Texture2DArray, (c_debugNames ? L"_loadedTexture_7" : nullptr), Context::LogFn);


                std::vector<unsigned char> pixels;
                for (const DX12Utils::TextureCache::Texture& texture : loadedTextureSlices)
                    pixels.insert(pixels.end(), texture.pixels.begin(), texture.pixels.end());

                DX12Utils::UploadTextureToGPUAndMakeMips(device, commandList, s_ubTracker, m_internal.texture__loadedTexture_7, pixels, size, desiredNumMips, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, LogFn);
            }
        }

        // _loadedTexture_8
        {
            if (!m_internal.texture__loadedTexture_8)
            {
                // Load the texture
                std::vector<DX12Utils::TextureCache::Texture> loadedTextureSlices;
                DX12Utils::DXGI_FORMAT_Info formatInfo = DX12Utils::Get_DXGI_FORMAT_Info(DXGI_FORMAT_R8_UNORM, Context::LogFn);
                DX12Utils::TextureCache::Type desiredType = DX12Utils::TextureCache::Type::U8;
                if (formatInfo.channelType == DX12Utils::DXGI_FORMAT_Info::ChannelType::_uint8_t)
                    desiredType = DX12Utils::TextureCache::Type::U8;
                else if (formatInfo.channelType == DX12Utils::DXGI_FORMAT_Info::ChannelType::_float)
                    desiredType = DX12Utils::TextureCache::Type::F32;
                else
                    Context::LogFn(LogLevel::Error, "Unhandled channel type for image: Textures/tellusim/128x128_l64_s16_resave.png");

                char loadedTextureFileName[1024];
                sprintf_s(loadedTextureFileName, "%lsassets/Textures/tellusim/128x128_l64_s16_resave.png", s_techniqueLocation.c_str());

                loadedTextureSlices.push_back(DX12Utils::TextureCache::GetAs(loadedTextureFileName, false, desiredType, formatInfo.sRGB, formatInfo.channelCount));
                DX12Utils::TextureCache::Texture& loadedTexture = loadedTextureSlices[0];
                if(!loadedTexture.Valid())
                    Context::LogFn(LogLevel::Error, "Could not load image: Textures/tellusim/128x128_l64_s16_resave.png");

                unsigned int size[3] = { (unsigned int)loadedTexture.width, (unsigned int)loadedTexture.height, 1 };

                static const unsigned int desiredNumMips = 1;

                // Create the texture
                dirty = true;
                m_internal.texture__loadedTexture_8_size[0] = size[0];
                m_internal.texture__loadedTexture_8_size[1] = size[1];
                m_internal.texture__loadedTexture_8_size[2] = size[2];
                m_internal.texture__loadedTexture_8_numMips = desiredNumMips;
                m_internal.texture__loadedTexture_8_format = DXGI_FORMAT_R8_UNORM;
                m_internal.texture__loadedTexture_8 = DX12Utils::CreateTexture(device, size, desiredNumMips, DXGI_FORMAT_R8_UNORM, m_internal.texture__loadedTexture_8_flags, D3D12_RESOURCE_STATE_COPY_DEST, DX12Utils::ResourceType::Texture2D, (c_debugNames ? L"_loadedTexture_8" : nullptr), Context::LogFn);


                std::vector<unsigned char> pixels;
                for (const DX12Utils::TextureCache::Texture& texture : loadedTextureSlices)
                    pixels.insert(pixels.end(), texture.pixels.begin(), texture.pixels.end());

                DX12Utils::UploadTextureToGPUAndMakeMips(device, commandList, s_ubTracker, m_internal.texture__loadedTexture_8, pixels, size, desiredNumMips, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, LogFn);
            }
        }

        // _loadedTexture_9
        {
            if (!m_internal.texture__loadedTexture_9)
            {
                // Load the texture
                std::vector<DX12Utils::TextureCache::Texture> loadedTextureSlices;
                DX12Utils::DXGI_FORMAT_Info formatInfo = DX12Utils::Get_DXGI_FORMAT_Info(DXGI_FORMAT_R8_UNORM, Context::LogFn);
                DX12Utils::TextureCache::Type desiredType = DX12Utils::TextureCache::Type::U8;
                if (formatInfo.channelType == DX12Utils::DXGI_FORMAT_Info::ChannelType::_uint8_t)
                    desiredType = DX12Utils::TextureCache::Type::U8;
                else if (formatInfo.channelType == DX12Utils::DXGI_FORMAT_Info::ChannelType::_float)
                    desiredType = DX12Utils::TextureCache::Type::F32;
                else
                    Context::LogFn(LogLevel::Error, "Unhandled channel type for image: Textures/Stable_Fiddusion/64x64x16 - S Rolloff - T Cosine - 0.0353.png");

                char loadedTextureFileName[1024];
                sprintf_s(loadedTextureFileName, "%lsassets/Textures/Stable_Fiddusion/64x64x16 - S Rolloff - T Cosine - 0.0353.png", s_techniqueLocation.c_str());

                loadedTextureSlices.push_back(DX12Utils::TextureCache::GetAs(loadedTextureFileName, false, desiredType, formatInfo.sRGB, formatInfo.channelCount));
                DX12Utils::TextureCache::Texture& loadedTexture = loadedTextureSlices[0];
                if(!loadedTexture.Valid())
                    Context::LogFn(LogLevel::Error, "Could not load image: Textures/Stable_Fiddusion/64x64x16 - S Rolloff - T Cosine - 0.0353.png");

                unsigned int size[3] = { (unsigned int)loadedTexture.width, (unsigned int)loadedTexture.height, 1 };

                static const unsigned int desiredNumMips = 1;

                // Create the texture
                dirty = true;
                m_internal.texture__loadedTexture_9_size[0] = size[0];
                m_internal.texture__loadedTexture_9_size[1] = size[1];
                m_internal.texture__loadedTexture_9_size[2] = size[2];
                m_internal.texture__loadedTexture_9_numMips = desiredNumMips;
                m_internal.texture__loadedTexture_9_format = DXGI_FORMAT_R8_UNORM;
                m_internal.texture__loadedTexture_9 = DX12Utils::CreateTexture(device, size, desiredNumMips, DXGI_FORMAT_R8_UNORM, m_internal.texture__loadedTexture_9_flags, D3D12_RESOURCE_STATE_COPY_DEST, DX12Utils::ResourceType::Texture2D, (c_debugNames ? L"_loadedTexture_9" : nullptr), Context::LogFn);


                std::vector<unsigned char> pixels;
                for (const DX12Utils::TextureCache::Texture& texture : loadedTextureSlices)
                    pixels.insert(pixels.end(), texture.pixels.begin(), texture.pixels.end());

                DX12Utils::UploadTextureToGPUAndMakeMips(device, commandList, s_ubTracker, m_internal.texture__loadedTexture_9, pixels, size, desiredNumMips, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, LogFn);
            }
        }

        // _ThresholdCB
        if (m_internal.constantBuffer__ThresholdCB == nullptr)
        {
            dirty = true;
            m_internal.constantBuffer__ThresholdCB = DX12Utils::CreateBuffer(device, 256, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COMMON, D3D12_HEAP_TYPE_DEFAULT, (c_debugNames ? L"_ThresholdCB" : nullptr), Context::LogFn);
        }

        // _SpatialFilterCB
        if (m_internal.constantBuffer__SpatialFilterCB == nullptr)
        {
            dirty = true;
            m_internal.constantBuffer__SpatialFilterCB = DX12Utils::CreateBuffer(device, 256, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COMMON, D3D12_HEAP_TYPE_DEFAULT, (c_debugNames ? L"_SpatialFilterCB" : nullptr), Context::LogFn);
        }

        // _TemporalFilterCB
        if (m_internal.constantBuffer__TemporalFilterCB == nullptr)
        {
            dirty = true;
            m_internal.constantBuffer__TemporalFilterCB = DX12Utils::CreateBuffer(device, 256, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COMMON, D3D12_HEAP_TYPE_DEFAULT, (c_debugNames ? L"_TemporalFilterCB" : nullptr), Context::LogFn);
        }

        // _AdjustBrightnessCB
        if (m_internal.constantBuffer__AdjustBrightnessCB == nullptr)
        {
            dirty = true;
            m_internal.constantBuffer__AdjustBrightnessCB = DX12Utils::CreateBuffer(device, 256, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COMMON, D3D12_HEAP_TYPE_DEFAULT, (c_debugNames ? L"_AdjustBrightnessCB" : nullptr), Context::LogFn);
        }
        EnsureDrawCallPSOsCreated(device, dirty);
    }

    bool Context::EnsureDrawCallPSOsCreated(ID3D12Device* device, bool dirty)
    {
        return true;
    }
};
