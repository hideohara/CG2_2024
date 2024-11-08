#pragma once

/*
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include "WinApp.h"
*/

#include "WinApp.h"
#include <array>
#include <d3d12.h>
#include <dxcapi.h>
#include <dxgi1_6.h>
#include <vector>
#include <wrl.h>

class DirectXCommon
{

public: // メンバ関数
    // 初期化
    void Initialize(WinApp* winApp);



private:
    void DeviceInitialize();
    void InitializeCommand();
    void CreateSwapchain();
    void CreateDepthBuffer();
    void InitializeDepthStencilView();
    void CreateDescriptorHeaps();
    void InitializeRenderTargetView();
    void CreateDxcCompiler();
    void CreateFence();
    void InitializeViewport();
    void InitializeScissorRect();
    void InitializeImGui();

    // DirectX12デバイス
    //Microsoft::WRL::ComPtr<ID3D12Device> device;
    // DXGIファクトリ
    //Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory;

    // WindowsAPI
    WinApp* winApp = nullptr;

    ID3D12Resource* CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height);
    ID3D12DescriptorHeap* CreateDescriptorHeap(
        D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);


    // スワップチェーンを生成する
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};

    ID3D12Device* device = nullptr;
    // DXGIファクトリーの生成
    IDXGIFactory7* dxgiFactory = nullptr;

    IDXGISwapChain4* swapChain = nullptr;
    //コマンドキューを生成する
    ID3D12CommandQueue* commandQueue = nullptr;
    // ビューポート
    D3D12_VIEWPORT viewport{};
    // シザー矩形
    D3D12_RECT scissorRect{};
    // 初期値0でFenceを作る
    ID3D12Fence* fence = nullptr;
    // dxcCompilerを初期化
    IDxcUtils* dxcUtils = nullptr;
    IDxcCompiler3* dxcCompiler = nullptr;
    // コマンドアロケータを生成する
    ID3D12CommandAllocator* commandAllocator = nullptr;

    // コマンドリストを生成する
    ID3D12GraphicsCommandList* commandList = nullptr;
    // SwapChainからResourceを引っ張ってくる
    ID3D12Resource* swapChainResources[2] = { nullptr };

    // DepthStencilTextureをウィンドウのサイズで作成
    ID3D12Resource* depthStencilResource;

    // RTVの設定
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};

    // RTV用のヒープでディスクリプタの数は2。RTVはShader内で触るものではないので、ShaderVisibleはfalse
    ID3D12DescriptorHeap* rtvDescriptorHeap;
    // SRV用のヒープでディスクリプタの数は128。SRVはShader内で触るものなので、ShaderVisibleはtrue
    ID3D12DescriptorHeap* srvDescriptorHeap;
    // DSV用のヒープでディスクリプタの数は1。DSVはShader内で触るものではないので、ShaderVisibleはfalse
    ID3D12DescriptorHeap* dsvDescriptorHeap;

};

