#include <stdio.h> 
// UNIT.02
#include "sprite.h"

#include "misc.h"

sprite::sprite(ID3D11Device* device)
{
    HRESULT hr = S_OK;

    // 頂点データの定義
    // 0           1
    // +-----------+
    // |           |
    // |           |
    // +-----------+
    // 2           3
    vertex vertices[] = {
        { DirectX::XMFLOAT3(-0.5, +0.5, 0), DirectX::XMFLOAT4(1, 1, 1, 1) },
        { DirectX::XMFLOAT3(+0.5, +0.5, 0), DirectX::XMFLOAT4(1, 0, 0, 1) },
        { DirectX::XMFLOAT3(-0.5, -0.5, 0), DirectX::XMFLOAT4(0, 1, 0, 1) },
        { DirectX::XMFLOAT3(+0.5, -0.5, 0), DirectX::XMFLOAT4(0, 0, 1, 1) },
    };
    // 頂点バッファの生成
    {
        D3D11_BUFFER_DESC buffer_desc = {};
        buffer_desc.ByteWidth = sizeof(vertices);
        buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
        buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        buffer_desc.MiscFlags = 0;
        buffer_desc.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA subresource_data = {};
        subresource_data.pSysMem = vertices;
        subresource_data.SysMemPitch = 0;
        subresource_data.SysMemSlicePitch = 0;

        hr = device->CreateBuffer(&buffer_desc, &subresource_data, &vertex_buffer);
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }

    // 頂点シェーダー＆頂点入力レイアウトの生成
    {
        FILE* fp = nullptr;
        fopen_s(&fp, "sprite_vs.cso", "rb");
        _ASSERT_EXPR_A(fp, "CSO File not found");

        fseek(fp, 0, SEEK_END);
        long cso_sz = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        unsigned char* cso_data = new unsigned char[cso_sz];
        fread(cso_data, cso_sz, 1, fp);
        fclose(fp);

        HRESULT hr = device->CreateVertexShader(
            cso_data,
            cso_sz,
            nullptr,
            &vertex_shader
        );
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };

        // 入力レイアウトの生成
        hr = device->CreateInputLayout(
            input_element_desc,
            ARRAYSIZE(input_element_desc),
            cso_data,
            cso_sz,
            &input_layout
        );
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        // 頂点シェーダーデータの後始末
        delete[] cso_data;
    }

    // ピクセルシェーダーの生成
    {
        FILE* fp = nullptr;
        fopen_s(&fp, "sprite_ps.cso", "rb");
        _ASSERT_EXPR_A(fp, "CSO File not found");

        fseek(fp, 0, SEEK_END);
        long cso_sz = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        unsigned char* cso_data = new unsigned char[cso_sz];
        fread(cso_data, cso_sz, 1, fp);
        fclose(fp);

        HRESULT hr = device->CreatePixelShader(
            cso_data,
            cso_sz,
            nullptr,
            &pixel_shader
        );
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        // ピクセルシェーダーデータの後始末
        delete[] cso_data;
    }

    // ラスタライザステートの生成
    {
        // ラスタライザステートを作成するための設定オプション
        D3D11_RASTERIZER_DESC rasterizer_desc = {}; //https://msdn.microsoft.com/en-us/library/windows/desktop/ff476198(v=vs.85).aspx
        rasterizer_desc.FillMode = D3D11_FILL_SOLID; //D3D11_FILL_WIREFRAME, D3D11_FILL_SOLID
        rasterizer_desc.CullMode = D3D11_CULL_NONE; //D3D11_CULL_NONE, D3D11_CULL_FRONT, D3D11_CULL_BACK   
        rasterizer_desc.FrontCounterClockwise = FALSE;
        rasterizer_desc.DepthBias = 0; //https://msdn.microsoft.com/en-us/library/windows/desktop/cc308048(v=vs.85).aspx
        rasterizer_desc.DepthBiasClamp = 0;
        rasterizer_desc.SlopeScaledDepthBias = 0;
        rasterizer_desc.DepthClipEnable = FALSE;
        rasterizer_desc.ScissorEnable = FALSE;
        rasterizer_desc.MultisampleEnable = FALSE;
        rasterizer_desc.AntialiasedLineEnable = FALSE;

        // ラスタライザステートの生成
        hr = device->CreateRasterizerState(
            &rasterizer_desc,
            &rasterizer_state
        );
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }
}

void sprite::render(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh, float angle, float r, float g, float b, float a) const
{
    D3D11_VIEWPORT viewport;
    UINT num_viewports = 1;
    immediate_context->RSGetViewports(&num_viewports, &viewport);
    float screen_width = viewport.Width;
    float screen_height = viewport.Height;

    // left-top
    float x0 = dx;
    float y0 = dy;
    // right-top
    float x1 = dx + dw;
    float y1 = dy;
    // left-bottom
    float x2 = dx;
    float y2 = dy + dh;
    // right-bottom
    float x3 = dx + dw;
    float y3 = dy + dh;

    float mx = dx + dw * 0.5f;
    float my = dy + dh * 0.5f;
    x0 -= mx;
    y0 -= my;
    x1 -= mx;
    y1 -= my;
    x2 -= mx;
    y2 -= my;
    x3 -= mx;
    y3 -= my;

    float rx, ry;
    float cos = cosf(angle * 0.01745f);
    float sin = sinf(angle * 0.01745f);
    rx = x0;
    ry = y0;
    x0 = cos * rx + -sin * ry;
    y0 = sin * rx + cos * ry;
    rx = x1;
    ry = y1;
    x1 = cos * rx + -sin * ry;
    y1 = sin * rx + cos * ry;
    rx = x2;
    ry = y2;
    x2 = cos * rx + -sin * ry;
    y2 = sin * rx + cos * ry;
    rx = x3;
    ry = y3;
    x3 = cos * rx + -sin * ry;
    y3 = sin * rx + cos * ry;

    x0 += mx;
    y0 += my;
    x1 += mx;
    y1 += my;
    x2 += mx;
    y2 += my;
    x3 += mx;
    y3 += my;

    x0 = 2.0f * x0 / screen_width - 1.0f;
    y0 = 1.0f - 2.0f * y0 / screen_height;
    x1 = 2.0f * x1 / screen_width - 1.0f;
    y1 = 1.0f - 2.0f * y1 / screen_height;
    x2 = 2.0f * x2 / screen_width - 1.0f;
    y2 = 1.0f - 2.0f * y2 / screen_height;
    x3 = 2.0f * x3 / screen_width - 1.0f;
    y3 = 1.0f - 2.0f * y3 / screen_height;

    HRESULT hr = S_OK;
    D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
    D3D11_MAPPED_SUBRESOURCE mapped_buffer;
    hr = immediate_context->Map(vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_buffer);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    vertex* vertices = static_cast<vertex*>(mapped_buffer.pData);
    vertices[0].position.x = x0;
    vertices[0].position.y = y0;
    vertices[1].position.x = x1;
    vertices[1].position.y = y1;
    vertices[2].position.x = x2;
    vertices[2].position.y = y2;
    vertices[3].position.x = x3;
    vertices[3].position.y = y3;
    vertices[0].position.z = vertices[1].position.z = vertices[2].position.z = vertices[3].position.z = 0.0f;

    DirectX::XMFLOAT4 color(r, g, b, a);
    DirectX::XMFLOAT4 color1(r - 1, g, b + 1, a);
    vertices[0].color = vertices[1].color = color;
    vertices[2].color = vertices[3].color = color1;

    immediate_context->Unmap(vertex_buffer, 0);

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

