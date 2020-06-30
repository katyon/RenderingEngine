#include <stdio.h>
#include "sprite.h"
#include "misc.h"

sprite::sprite(ID3D11Device* device)
{
    HRESULT hr = S_OK;

    vertex vertices[] = {
        {DirectX::XMFLOAT3(-0.5,+0.5,0),DirectX::XMFLOAT4(1,1,1,1)},
        {DirectX::XMFLOAT3(+0.5,+0.5,0),DirectX::XMFLOAT4(1,0,0,1)},
        {DirectX::XMFLOAT3(-0.5,-0.5,0),DirectX::XMFLOAT4(0,1,0,1)},
        {DirectX::XMFLOAT3(+0.5,-0.5,0),DirectX::XMFLOAT4(0,0,1,1)},
    };
    // 頂点バッファの作成
    {
        // 頂点バッファを作成するための設定オプション
        D3D11_BUFFER_DESC buffer_desc = {};
        buffer_desc.ByteWidth = sizeof(vertices);
        buffer_desc.Usage = D3D11_USAGE_DEFAULT;
        buffer_desc.BindFlags = 0;
        buffer_desc.CPUAccessFlags = 0;
        buffer_desc.MiscFlags = 0;
        buffer_desc.StructureByteStride = 0;
        // 頂点バッファに頂点
        D3D11_SUBRESOURCE_DATA subresource_data = {};
        subresource_data.pSysMem = vertices;
        subresource_data.SysMemPitch = 0;
        subresource_data.SysMemSlicePitch = 0;
        // 頂点バッファオブジェクトの生成
        hr = device->CreateBuffer(&buffer_desc, &subresource_data, &vertex_buffer);
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }

    // 頂点シェーダー&頂点入力レイアウトの生成
    {
        // 頂点シェーダーファイルを開く
        FILE* fp = nullptr;
        fopen_s(&fp, "sprite_vs.cso", "rb");
        _ASSERT_EXPR_A(fp, "CSO File not found");

        // 頂点シェーダーファイルのサイズを求める
        fseek(fp, 0, SEEK_END);
        long cso_sz = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        // メモリ上に頂点シェーダーデータを格納する領域を用意する
        unsigned char* cso_data = new unsigned char[cso_sz];
        fread(cso_data, cso_sz, 1, fp);
        fclose(fp);

        // 頂点シェーダーデータを基に頂点シェーダーオブジェクトを生成する
        HRESULT hr = device->CreateVertexShader(
            cso_data,       // 頂点シェーダーデータのポインタ
            cso_sz,         // 頂点シェーダーデータサイズ
            nullptr,
            &vertex_shader  // 頂点シェーダーオブジェクトのポインタの格納先
        );
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        // GPUに頂点データの内容を教えてあげるための設定
        D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
        {
            { "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        };
        // 入力レイアウトの生成
        hr = device->CreateInputLayout(
            input_element_desc,             // 頂点データの内容
            ARRAYSIZE(input_element_desc),  // 頂点データの要素数
            cso_data,                       // 頂点シェーダーデータ（input_element_descの内容と sprite_vs.hlslの内容に不一致がないかチェックするため）
            cso_sz,                         // 頂点シェーダーデータサイズ
            &input_layout                   // 入力レイアウトオブジェクトのポインタの格納先
        );
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        // 頂点シェーダーデータの後始末
        delete[] cso_data;
    }

    // ピクセルシェーダーも生成
    {
        // ピクセルシェーダーファイルを開く(sprite_ps.hlsl をコンパイルしてできたファイル)
        FILE* fp = nullptr;
        fopen_s(&fp, "sprite_ps.cso", "rb");
        _ASSERT_EXPR_A(fp, "CSO File not found");

        // ピクセルシェーダーファイルのサイズを求める
        fseek(fp, 0, SEEK_END);
        long cso_sz = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        // メモリ上にピクセルシェーダーデータを格納する領域を用意する
        unsigned char* cso_data = new unsigned char[cso_sz];
        fread(cso_data, cso_sz, 1, fp);	// 用意した領域にデータを読み込む
        fclose(fp);	// ファイルを閉じる

        // ピクセルシェーダーの生成
        HRESULT hr = device->CreatePixelShader(
            cso_data,		// ピクセルシェーダーデータのポインタ
            cso_sz,			// ピクセルシェーダーデータサイズ
            nullptr,
            &pixel_shader	// ピクセルシェーダーオブジェクトのポインタの格納先
        );
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        // ピクセルシェーダーデータの後始末
        delete[] cso_data;
    }

    // ラスタライザステートの生成
    {
        // ラスタライザステートを作成するための設定
        D3D11_RASTERIZER_DESC rasterizer_desc = {};
        rasterizer_desc.FillMode = D3D11_FILL_SOLID;
        rasterizer_desc.CullMode = D3D11_CULL_NONE;
        rasterizer_desc.FrontCounterClockwise = FALSE;
        rasterizer_desc.DepthBias = 0;
        rasterizer_desc.DepthBiasClamp = 0;
        rasterizer_desc.SlopeScaledDepthBias = 0;
        rasterizer_desc.DepthClipEnable = 0;
        rasterizer_desc.ScissorEnable = FALSE;
        rasterizer_desc.MultisampleEnable = FALSE;
        rasterizer_desc.AntialiasedLineEnable = FALSE;

        // ラスタライザステートの生成
        hr = device->CreateRasterizerState(
            &rasterizer_desc,
            &rasterizer_state);
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }
}

void sprite::render(ID3D11DeviceContext* immediate_context) const
{
    UINT stride = sizeof(vertex);
    UINT offset = 0;
    immediate_context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);
    immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    immediate_context->IASetInputLayout(input_layout);

    immediate_context->RSSetState(rasterizer_state);

    immediate_context->VSSetShader(vertex_shader, nullptr, 0);
    immediate_context->PSSetShader(pixel_shader, nullptr, 0);

    immediate_context->Draw(4, 0);
}