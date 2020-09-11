#include "shader.h"
#include "misc.h"
#include "geometric_primitive.h"

geometric_primitive::geometric_primitive(ID3D11Device* device)
{
    HRESULT hr = S_OK;

    D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    create_vs_from_cso(device, "geometric_primitive_vs.cso", vertex_shader.GetAddressOf(), input_layout.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
    create_ps_from_cso(device, "geometric_primitive_ps.cso", pixel_shader.GetAddressOf());

    // ラスタライザーステートの作成(通常)
    {
        D3D11_RASTERIZER_DESC rasterizer_desc = {};
        rasterizer_desc.FillMode = D3D11_FILL_SOLID;
        rasterizer_desc.CullMode = D3D11_CULL_BACK;
        rasterizer_desc.FrontCounterClockwise = FALSE;
        rasterizer_desc.DepthBias = 0;
        rasterizer_desc.DepthBiasClamp = 0;
        rasterizer_desc.SlopeScaledDepthBias = 0;
        rasterizer_desc.DepthClipEnable = TRUE;
        rasterizer_desc.ScissorEnable = FALSE;
        rasterizer_desc.MultisampleEnable = FALSE;
        rasterizer_desc.AntialiasedLineEnable = FALSE;
        hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_states[0].GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }
    // ラスタライザーステートの作成(ワイヤーフレーム)
    {
        D3D11_RASTERIZER_DESC rasterizer_desc = {};
        rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME;
        rasterizer_desc.CullMode = D3D11_CULL_BACK;
        rasterizer_desc.FrontCounterClockwise = FALSE;
        rasterizer_desc.DepthBias = 0;
        rasterizer_desc.DepthBiasClamp = 0;
        rasterizer_desc.SlopeScaledDepthBias = 0;
        rasterizer_desc.DepthClipEnable = TRUE;
        rasterizer_desc.ScissorEnable = FALSE;
        rasterizer_desc.MultisampleEnable = FALSE;
        rasterizer_desc.AntialiasedLineEnable = FALSE;
        hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_states[1].GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }
    // デプスステンシルステートの作成
    {
        D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
        depth_stencil_desc.DepthEnable = TRUE;
        depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
        depth_stencil_desc.StencilEnable = FALSE;
        depth_stencil_desc.StencilReadMask = 0xFF;
        depth_stencil_desc.StencilWriteMask = 0xFF;
        depth_stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        depth_stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
        depth_stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        depth_stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        depth_stencil_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        depth_stencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
        depth_stencil_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        depth_stencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        hr = device->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_state.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }
    // コンスタントバッファーの作成
    {
        D3D11_BUFFER_DESC buffer_desc = {};
        buffer_desc.ByteWidth = sizeof(cbuffer);
        buffer_desc.Usage = D3D11_USAGE_DEFAULT;
        buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        buffer_desc.CPUAccessFlags = 0;
        buffer_desc.MiscFlags = 0;
        buffer_desc.StructureByteStride = 0;
        hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffer.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }

    // キューブのメッシュの作成
    vertex vertices[24] = {};
    u_int indices[36] = {};

    int face;

    // top-side
    // 0---------1
    // |         |
    // |   -Y    |
    // |         |
    // 2---------3
    face = 0;
    vertices[face * 4 + 0].position = DirectX::XMFLOAT3(-0.5f, +0.5f, +0.5f);
    vertices[face * 4 + 1].position = DirectX::XMFLOAT3(+0.5f, +0.5f, +0.5f);
    vertices[face * 4 + 2].position = DirectX::XMFLOAT3(-0.5f, +0.5f, -0.5f);
    vertices[face * 4 + 3].position = DirectX::XMFLOAT3(+0.5f, +0.5f, -0.5f);
    vertices[face * 4 + 0].normal = DirectX::XMFLOAT3(+0.0f, +1.0f, +0.0f);
    vertices[face * 4 + 1].normal = DirectX::XMFLOAT3(+0.0f, +1.0f, +0.0f);
    vertices[face * 4 + 2].normal = DirectX::XMFLOAT3(+0.0f, +1.0f, +0.0f);
    vertices[face * 4 + 3].normal = DirectX::XMFLOAT3(+0.0f, +1.0f, +0.0f);
    indices[face * 6 + 0] = face * 4 + 0;
    indices[face * 6 + 1] = face * 4 + 1;
    indices[face * 6 + 2] = face * 4 + 2;
    indices[face * 6 + 3] = face * 4 + 1;
    indices[face * 6 + 4] = face * 4 + 3;
    indices[face * 6 + 5] = face * 4 + 2;

    // bottom-side
    // 0---------1
    // |         |
    // |   -Y    |
    // |         |
    // 2---------3
    face += 1;
    vertices[face * 4 + 0].position = DirectX::XMFLOAT3(-0.5f, -0.5f, +0.5f);
    vertices[face * 4 + 1].position = DirectX::XMFLOAT3(+0.5f, -0.5f, +0.5f);
    vertices[face * 4 + 2].position = DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f);
    vertices[face * 4 + 3].position = DirectX::XMFLOAT3(+0.5f, -0.5f, -0.5f);
    vertices[face * 4 + 0].normal = DirectX::XMFLOAT3(+0.0f, -1.0f, +0.0f);
    vertices[face * 4 + 1].normal = DirectX::XMFLOAT3(+0.0f, -1.0f, +0.0f);
    vertices[face * 4 + 2].normal = DirectX::XMFLOAT3(+0.0f, -1.0f, +0.0f);
    vertices[face * 4 + 3].normal = DirectX::XMFLOAT3(+0.0f, -1.0f, +0.0f);
    indices[face * 6 + 0] = face * 4 + 0;
    indices[face * 6 + 1] = face * 4 + 2;
    indices[face * 6 + 2] = face * 4 + 1;
    indices[face * 6 + 3] = face * 4 + 1;
    indices[face * 6 + 4] = face * 4 + 2;
    indices[face * 6 + 5] = face * 4 + 3;

    // front-side
    // 0---------1
    // |         |
    // |   +Z    |
    // |         |
    // 2---------3
    face += 1;
    vertices[face * 4 + 0].position = DirectX::XMFLOAT3(-0.5f, +0.5f, -0.5f);
    vertices[face * 4 + 1].position = DirectX::XMFLOAT3(+0.5f, +0.5f, -0.5f);
    vertices[face * 4 + 2].position = DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f);
    vertices[face * 4 + 3].position = DirectX::XMFLOAT3(+0.5f, -0.5f, -0.5f);
    vertices[face * 4 + 0].normal = DirectX::XMFLOAT3(+0.0f, +0.0f, -1.0f);
    vertices[face * 4 + 1].normal = DirectX::XMFLOAT3(+0.0f, +0.0f, -1.0f);
    vertices[face * 4 + 2].normal = DirectX::XMFLOAT3(+0.0f, +0.0f, -1.0f);
    vertices[face * 4 + 3].normal = DirectX::XMFLOAT3(+0.0f, +0.0f, -1.0f);
    indices[face * 6 + 0] = face * 4 + 0;
    indices[face * 6 + 1] = face * 4 + 1;
    indices[face * 6 + 2] = face * 4 + 2;
    indices[face * 6 + 3] = face * 4 + 1;
    indices[face * 6 + 4] = face * 4 + 3;
    indices[face * 6 + 5] = face * 4 + 2;

    // back-side
    // 0---------1
    // |         |
    // |   +Z    |
    // |         |
    // 2---------3
    face += 1;
    vertices[face * 4 + 0].position = DirectX::XMFLOAT3(-0.5f, +0.5f, +0.5f);
    vertices[face * 4 + 1].position = DirectX::XMFLOAT3(+0.5f, +0.5f, +0.5f);
    vertices[face * 4 + 2].position = DirectX::XMFLOAT3(-0.5f, -0.5f, +0.5f);
    vertices[face * 4 + 3].position = DirectX::XMFLOAT3(+0.5f, -0.5f, +0.5f);
    vertices[face * 4 + 0].normal = DirectX::XMFLOAT3(+0.0f, +0.0f, +1.0f);
    vertices[face * 4 + 1].normal = DirectX::XMFLOAT3(+0.0f, +0.0f, +1.0f);
    vertices[face * 4 + 2].normal = DirectX::XMFLOAT3(+0.0f, +0.0f, +1.0f);
    vertices[face * 4 + 3].normal = DirectX::XMFLOAT3(+0.0f, +0.0f, +1.0f);
    indices[face * 6 + 0] = face * 4 + 0;
    indices[face * 6 + 1] = face * 4 + 2;
    indices[face * 6 + 2] = face * 4 + 1;
    indices[face * 6 + 3] = face * 4 + 1;
    indices[face * 6 + 4] = face * 4 + 2;
    indices[face * 6 + 5] = face * 4 + 3;

    // right-side
    // 0---------1
    // |         |      
    // |   -X    |
    // |         |
    // 2---------3
    face += 1;
    vertices[face * 4 + 0].position = DirectX::XMFLOAT3(+0.5f, +0.5f, -0.5f);
    vertices[face * 4 + 1].position = DirectX::XMFLOAT3(+0.5f, +0.5f, +0.5f);
    vertices[face * 4 + 2].position = DirectX::XMFLOAT3(+0.5f, -0.5f, -0.5f);
    vertices[face * 4 + 3].position = DirectX::XMFLOAT3(+0.5f, -0.5f, +0.5f);
    vertices[face * 4 + 0].normal = DirectX::XMFLOAT3(+1.0f, +0.0f, +0.0f);
    vertices[face * 4 + 1].normal = DirectX::XMFLOAT3(+1.0f, +0.0f, +0.0f);
    vertices[face * 4 + 2].normal = DirectX::XMFLOAT3(+1.0f, +0.0f, +0.0f);
    vertices[face * 4 + 3].normal = DirectX::XMFLOAT3(+1.0f, +0.0f, +0.0f);
    indices[face * 6 + 0] = face * 4 + 0;
    indices[face * 6 + 1] = face * 4 + 1;
    indices[face * 6 + 2] = face * 4 + 2;
    indices[face * 6 + 3] = face * 4 + 1;
    indices[face * 6 + 4] = face * 4 + 3;
    indices[face * 6 + 5] = face * 4 + 2;

    // left-side
    // 0---------1
    // |         |      
    // |   -X    |
    // |         |
    // 2---------3
    face += 1;
    vertices[face * 4 + 0].position = DirectX::XMFLOAT3(-0.5f, +0.5f, -0.5f);
    vertices[face * 4 + 1].position = DirectX::XMFLOAT3(-0.5f, +0.5f, +0.5f);
    vertices[face * 4 + 2].position = DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f);
    vertices[face * 4 + 3].position = DirectX::XMFLOAT3(-0.5f, -0.5f, +0.5f);
    vertices[face * 4 + 0].normal = DirectX::XMFLOAT3(-1.0f, +0.0f, +0.0f);
    vertices[face * 4 + 1].normal = DirectX::XMFLOAT3(-1.0f, +0.0f, +0.0f);
    vertices[face * 4 + 2].normal = DirectX::XMFLOAT3(-1.0f, +0.0f, +0.0f);
    vertices[face * 4 + 3].normal = DirectX::XMFLOAT3(-1.0f, +0.0f, +0.0f);
    indices[face * 6 + 0] = face * 4 + 0;
    indices[face * 6 + 1] = face * 4 + 2;
    indices[face * 6 + 2] = face * 4 + 1;
    indices[face * 6 + 3] = face * 4 + 1;
    indices[face * 6 + 4] = face * 4 + 2;
    indices[face * 6 + 5] = face * 4 + 3;

    create_buffers(device, vertices, 24, indices, 36);
}

void geometric_primitive::create_buffers(ID3D11Device* device, vertex* vertices, int num_vertices, u_int* indices, int num_indices)
{
    HRESULT hr;
    {
        D3D11_BUFFER_DESC buffer_desc = {};
        D3D11_SUBRESOURCE_DATA subresource_data = {};

        buffer_desc.ByteWidth = sizeof(vertex) * num_vertices;
        buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
        buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        buffer_desc.CPUAccessFlags = 0;
        buffer_desc.MiscFlags = 0;
        buffer_desc.StructureByteStride = 0;
        subresource_data.pSysMem = vertices;
        subresource_data.SysMemPitch = 0;
        subresource_data.SysMemSlicePitch = 0;

        hr = device->CreateBuffer(&buffer_desc, &subresource_data, vertex_buffer.ReleaseAndGetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }
    {
        D3D11_BUFFER_DESC buffer_desc = {};
        D3D11_SUBRESOURCE_DATA subresource_data = {};

        buffer_desc.ByteWidth = sizeof(u_int) * num_indices;
        buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
        buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        buffer_desc.CPUAccessFlags = 0;
        buffer_desc.MiscFlags = 0;
        buffer_desc.StructureByteStride = 0;
        subresource_data.pSysMem = indices;
        subresource_data.SysMemPitch = 0;
        subresource_data.SysMemSlicePitch = 0;
        hr = device->CreateBuffer(&buffer_desc, &subresource_data, index_buffer.ReleaseAndGetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }
}

void geometric_primitive::render(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT4X4& world_view_projection,
                                const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& light_direction, const DirectX::XMFLOAT4& material_color, bool wireframe)
{
    cbuffer data;
    data.world_view_projection = world_view_projection;
    data.world = world;
    data.light_direction = light_direction;
    data.material_color = material_color;
    immediate_context->UpdateSubresource(constant_buffer.Get(), 0, 0, &data, 0, 0);
    immediate_context->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());

    u_int stride = sizeof(vertex);
    u_int offset = 0;
    immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
    immediate_context->IASetIndexBuffer(index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    immediate_context->IASetInputLayout(input_layout.Get());

    immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
    immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);

    immediate_context->OMSetDepthStencilState(depth_stencil_state.Get(), 1);
    if (wireframe)
    {
        immediate_context->RSSetState(rasterizer_states[1].Get());
    }
    else
    {
        immediate_context->RSSetState(rasterizer_states[0].Get());
    }

    D3D11_BUFFER_DESC buffer_desc;
    index_buffer->GetDesc(&buffer_desc);
    immediate_context->DrawIndexed(buffer_desc.ByteWidth / sizeof(u_int), 0, 0);
}
