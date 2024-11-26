#pragma once

#include "DirectXCommon.h"


class SpriteCommon
{
public: // メンバ関数
    // 初期化
    void Initialize(DirectXCommon* dxCommon);

    void Finish();

    DirectXCommon* GetDxCommon() const { return dxCommon_; }

private:
    // DirectXCommonのポインタ
    DirectXCommon* dxCommon_;

    ID3D12RootSignature* rootSignature = nullptr;
    ID3DBlob* signatureBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;

    D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};

    D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
    D3D12_BLEND_DESC blendDesc{};
    D3D12_RASTERIZER_DESC rasterizerDesc{};
    D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
    IDxcBlob* vertexShaderBlob = nullptr;
    IDxcBlob* pixelShaderBlob = nullptr;
    ID3D12PipelineState* graphicsPipelineState = nullptr;

    // RootSignatureの生成
    void CreateRootSignature();

    // InputLayoutの生成
    void CreateInputLayout();

    // BlendStateの設定
    void CreateBlendState();

    // RasiterzerStateの設定
    void CreateRasiterzerState();

    // DepthStencilStateの設定
    void CreateDepthStencilState();

    // Vertex Shaderをコンパイルする
    void CreateVertexShader();

    // Pixel Shaderをコンパイルする
    void CreatePixelShader();


    // GraphicsPipelineの生成
    void CreateGraphicsPipeline();

};

