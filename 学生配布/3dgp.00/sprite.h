#pragma once

#include <d3d11.h>
#include <directxmath.h>

class sprite
{
private:
    ID3D11VertexShader* vertex_shader = nullptr;
    ID3D11PixelShader* pixel_shader = nullptr;
    ID3D11InputLayout* input_layout = nullptr;

    ID3D11Buffer* vertex_buffer = nullptr;

    ID3D11RasterizerState* rasterizer_state = nullptr;

    ID3D11ShaderResourceView* shader_resource_view = nullptr;
    D3D11_TEXTURE2D_DESC texture2d_desc;
    ID3D11SamplerState* sampler_state = nullptr;

    ID3D11DepthStencilState* depth_stencil_state = nullptr;
    //ID3D11BlendState* blend_state = nullptr;
public:
    struct vertex
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT4 color;
        DirectX::XMFLOAT2 texcoord;
    };

    sprite(ID3D11Device* device, const wchar_t* file_name);
    virtual ~sprite()
    {
        if (vertex_shader != nullptr)
        {
            vertex_shader->Release();
            vertex_shader = nullptr;
        }
        if (pixel_shader != nullptr)
        {
            pixel_shader->Release();
            pixel_shader = nullptr;
        }
        if (input_layout != nullptr)
        {
            input_layout->Release();
            input_layout = nullptr;
        }
        if (vertex_buffer != nullptr)
        {
            vertex_buffer->Release();
            vertex_buffer = nullptr;
        }
        if (rasterizer_state != nullptr)
        {
            rasterizer_state->Release();
            rasterizer_state = nullptr;
        }
    }
    void render(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh, float sx, float sy, float sw, float sh, float angle, float r, float g, float b, float a) const;
};