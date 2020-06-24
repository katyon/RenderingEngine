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
    }
}

