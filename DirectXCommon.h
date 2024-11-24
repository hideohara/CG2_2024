#pragma once

/*
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <dxcapi.h>

#include "WinApp.h"


#include "externals/DirectXTex/DirectXTex.h"
*/

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include "WinApp.h"
#include "array"
#include <dxcapi.h>
#include <string>
#include "externals/DirectXTex/DirectXTex.h"

class DirectXCommon
{

public: // メンバ関数

    template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

    // 初期化
    void Initialize(WinApp* winApp);

    // 描画前処理
    void PreDraw();

    // 描画後処理
    void PostDraw();


    // デスクリプタヒープを作成する
    ID3D12DescriptorHeap* CreateDescriptorHeap(
        D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

    /// <summary>
    /// 指定番号のCPUデスクリプタハンドルを取得する
    /// </summary>
    static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index);

    /// <summary>
    /// 指定番号のGPUデスクリプタハンドルを取得する
    /// </summary>
    static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index);

    /// <summary>
    /// SRVの指定番号のCPUデスクリプタハンドルを取得する
    /// </summary>
    D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCPUDescriptorHandle(uint32_t index);

    /// <summary>
    /// SRVの指定番号のGPUデスクリプタハンドルを取得する
    /// </summary>
    D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUDescriptorHandle(uint32_t index);



    // getter
    ID3D12Device* GetDevice() const { return device; }
    ID3D12GraphicsCommandList* GetCommandList() const { return commandList; }


    // シェーダーのコンパイル
    IDxcBlob* CompileShader(
        const std::wstring& filePath,
        const wchar_t* profile);


    /// <summary>
    /// バッファリソースの生成
    /// </summary>
    ID3D12Resource* CreateBufferResource(size_t sizeInBytes);

    /// <summary>
    /// テクスチャリソースの生成
    /// </summary>
    //Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(
    //    ID3D12Device* device, const DirectX::TexMetadata& metadata);
    ID3D12Resource* CreateTextureResource(const DirectX::TexMetadata& metadata);

    /// <summary>
    /// テクスチャデータの転送
    /// </summary>
    void UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);

    /// <summary>
    /// テクスチャファイルの読み込み
    /// </summary>
    /// <param name="filePath">テクスチャファイルのパス</param>
    /// <returns>画像イメージデータ</returns>
    static DirectX::ScratchImage LoadTexture(const std::string& filePath);

    void Finish();

private:
    // デバイス生成
    void DeviceInitialize();

    // コマンド関連の初期化
    void CommandRelatedInitialize();

    // スワップチェーンの生成
    void SwapChainGenerate();


    // 深度バッファの生成
    void DepthBufferGenerate();

    // 各種デスクリプターヒープの生成
    void VariousDescriptorHeapGenerate();

    // レンダーターゲットビューの初期化
    void RenderTargetViewInitialize();

    // 深度ステンシルビューの初期化
    void DepthStencilViewInitialize();

    // フェンスの初期化
    void FenceInitialize();

    // ビューポート矩形の初期化
    void ViewportRectInitialize();

    // シザリング矩形の初期化
    void ScissoringRectInitialize();

    // DXCコンパイラの生成
    void DXCCompilerGenerate();

    // ImGuiの初期化
    void ImGuiInitialize();



private:
    // WindowsAPI
    WinApp* winApp = nullptr;

    ID3D12Device* device = nullptr;
    IDXGIFactory7* dxgiFactory = nullptr;

    ID3D12CommandQueue* commandQueue = nullptr;
    ID3D12CommandAllocator* commandAllocator = nullptr;
    ID3D12GraphicsCommandList* commandList = nullptr;

    // DescriptorSizeを取得しておく
    uint32_t desriptorSizeSRV;
    uint32_t desriptorSizeRTV; 
    uint32_t desriptorSizeDSV; 

    // RTV用のヒープ
    ID3D12DescriptorHeap* rtvDescriptorHeap; 
    // SRV用のヒープ
    ID3D12DescriptorHeap* srvDescriptorHeap;
    // DSV用のヒープ
    ID3D12DescriptorHeap* dsvDescriptorHeap; 

    IDXGISwapChain4* swapChain = nullptr;
    ID3D12Resource* swapChainResources[2] = { nullptr };

    // 深度ステンシルビュー ???????
    ID3D12Resource* depthStencilResource = nullptr;

    ID3D12Fence* fence = nullptr;

    // ビューポート
    D3D12_VIEWPORT viewport{};

    // シザー矩形
    D3D12_RECT scissorRect{};

    // dxcCompilerを初期化
    IDxcUtils* dxcUtils = nullptr;
    IDxcCompiler3* dxcCompiler = nullptr;  
    IDxcIncludeHandler* includeHandler = nullptr;

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};

    // RTVの設定
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};

    // RTVを2つ作るのでディスクリプタを2つ用意
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];

    // TransitionBarrierの設定
    D3D12_RESOURCE_BARRIER barrier{};

    uint64_t fenceValue = 0;
    // フェンスイベント
    HANDLE fenceEvent;

    // 使用するアダプタ用の変数。最初にnullptrを入れておく
    IDXGIAdapter4* useAdapter = nullptr;
};

