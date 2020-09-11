#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>
#include <string>

class sprite
{
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_state;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
	D3D11_TEXTURE2D_DESC texture2d_desc;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_state;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_state;

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

	}

	void render(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh, float sx, float sy, float sw, float sh, float angle = 0, float r = 1, float g = 1, float b = 1, float a = 1) const;
	void textout(ID3D11DeviceContext* immediate_context, std::string s, float x, float y, float w, float h, float r = 1, float g = 1, float b = 1, float a = 1) const;
};

class sprite_batch
{
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_state;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
	D3D11_TEXTURE2D_DESC texture2d_desc;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_state;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_state;

	const size_t MAX_INSTANCES = 256;
	struct instance
	{
		DirectX::XMFLOAT4X4 ndc_transform;
		DirectX::XMFLOAT4 texcoord_transform;
		DirectX::XMFLOAT4 color;
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> instance_buffer;

public:
	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texcoord;
	};

	sprite_batch(ID3D11Device* device, const wchar_t* file_name, size_t max_instance = 256);

	void begin(ID3D11DeviceContext* immediate_context);
	void render(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh, float sx, float sy, float sw, float sh, float angle/*degree*/, float r, float g, float b, float a);
	void end(ID3D11DeviceContext* immediate_context);

private:
	D3D11_VIEWPORT viewport;

	UINT count_instance = 0;
	instance* instances = nullptr;
};