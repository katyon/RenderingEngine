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
    // ���_�o�b�t�@�̍쐬
    {
        // ���_�o�b�t�@���쐬���邽�߂̐ݒ�I�v�V����
        D3D11_BUFFER_DESC buffer_desc = {};
        buffer_desc.ByteWidth = sizeof(vertices);
        buffer_desc.Usage = D3D11_USAGE_DEFAULT;
        buffer_desc.BindFlags = 0;
        buffer_desc.CPUAccessFlags = 0;
        buffer_desc.MiscFlags = 0;
        buffer_desc.StructureByteStride = 0;
        // ���_�o�b�t�@�ɒ��_
        D3D11_SUBRESOURCE_DATA subresource_data = {};
        subresource_data.pSysMem = vertices;
        subresource_data.SysMemPitch = 0;
        subresource_data.SysMemSlicePitch = 0;
        // ���_�o�b�t�@�I�u�W�F�N�g�̐���
        hr = device->CreateBuffer(&buffer_desc, &subresource_data, &vertex_buffer);
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }

    // ���_�V�F�[�_�[&���_���̓��C�A�E�g�̐���
    {
        // ���_�V�F�[�_�[�t�@�C�����J��
        FILE* fp = nullptr;
        fopen_s(&fp, "sprite_vs.cso", "rb");
        _ASSERT_EXPR_A(fp, "CSO File not found");

        // ���_�V�F�[�_�[�t�@�C���̃T�C�Y�����߂�
        fseek(fp, 0, SEEK_END);
        long cso_sz = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        // ��������ɒ��_�V�F�[�_�[�f�[�^���i�[����̈��p�ӂ���
        unsigned char* cso_data = new unsigned char[cso_sz];
        fread(cso_data, cso_sz, 1, fp);
        fclose(fp);

        // ���_�V�F�[�_�[�f�[�^����ɒ��_�V�F�[�_�[�I�u�W�F�N�g�𐶐�����
        HRESULT hr = device->CreateVertexShader(
            cso_data,       // ���_�V�F�[�_�[�f�[�^�̃|�C���^
            cso_sz,         // ���_�V�F�[�_�[�f�[�^�T�C�Y
            nullptr,
            &vertex_shader  // ���_�V�F�[�_�[�I�u�W�F�N�g�̃|�C���^�̊i�[��
        );
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        // GPU�ɒ��_�f�[�^�̓��e�������Ă����邽�߂̐ݒ�
        D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
        {
            { "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        };
        // ���̓��C�A�E�g�̐���
        hr = device->CreateInputLayout(
            input_element_desc,             // ���_�f�[�^�̓��e
            ARRAYSIZE(input_element_desc),  // ���_�f�[�^�̗v�f��
            cso_data,                       // ���_�V�F�[�_�[�f�[�^�iinput_element_desc�̓��e�� sprite_vs.hlsl�̓��e�ɕs��v���Ȃ����`�F�b�N���邽�߁j
            cso_sz,                         // ���_�V�F�[�_�[�f�[�^�T�C�Y
            &input_layout                   // ���̓��C�A�E�g�I�u�W�F�N�g�̃|�C���^�̊i�[��
        );
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        // ���_�V�F�[�_�[�f�[�^�̌�n��
        delete[] cso_data;
    }

    // �s�N�Z���V�F�[�_�[������
    {
        // �s�N�Z���V�F�[�_�[�t�@�C�����J��(sprite_ps.hlsl ���R���p�C�����Ăł����t�@�C��)
        FILE* fp = nullptr;
        fopen_s(&fp, "sprite_ps.cso", "rb");
        _ASSERT_EXPR_A(fp, "CSO File not found");

        // �s�N�Z���V�F�[�_�[�t�@�C���̃T�C�Y�����߂�
        fseek(fp, 0, SEEK_END);
        long cso_sz = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        // ��������Ƀs�N�Z���V�F�[�_�[�f�[�^���i�[����̈��p�ӂ���
        unsigned char* cso_data = new unsigned char[cso_sz];
        fread(cso_data, cso_sz, 1, fp);	// �p�ӂ����̈�Ƀf�[�^��ǂݍ���
        fclose(fp);	// �t�@�C�������

        // �s�N�Z���V�F�[�_�[�̐���
        HRESULT hr = device->CreatePixelShader(
            cso_data,		// �s�N�Z���V�F�[�_�[�f�[�^�̃|�C���^
            cso_sz,			// �s�N�Z���V�F�[�_�[�f�[�^�T�C�Y
            nullptr,
            &pixel_shader	// �s�N�Z���V�F�[�_�[�I�u�W�F�N�g�̃|�C���^�̊i�[��
        );
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        // �s�N�Z���V�F�[�_�[�f�[�^�̌�n��
        delete[] cso_data;
    }

    // ���X�^���C�U�X�e�[�g�̐���
    {
        // ���X�^���C�U�X�e�[�g���쐬���邽�߂̐ݒ�
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

        // ���X�^���C�U�X�e�[�g�̐���
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