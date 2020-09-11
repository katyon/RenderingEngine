#pragma once

// UNIT.10
#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>

class geometric_primitive
{
public:
	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
	};
	struct cbuffer
	{
		DirectX::XMFLOAT4X4 world_view_projection;
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4 material_color;
		DirectX::XMFLOAT4 light_direction;
	};

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_states[2];
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_state;

public:
	geometric_primitive(ID3D11Device* device);
	virtual ~geometric_primitive() {}

	void render(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT4X4& world_view_projection, const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& light_direction, const DirectX::XMFLOAT4& material_color, bool wireframe = false);

protected:
	void create_buffers(ID3D11Device* device, vertex* vertices, int num_vertices, u_int* indices, int num_indices);
};

