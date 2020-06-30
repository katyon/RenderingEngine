#include "framework.h"

bool framework::initialize()
{
    HRESULT hr = S_OK;

    // 画面サイズを取得する。
    RECT rc;
    GetClientRect(hwnd, &rc);
    UINT screen_width = rc.right - rc.left;
    UINT screen_height = rc.bottom - rc.top;

    // デバイス&スワップチェーンの生成
    {
        UINT create_device_flags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
        create_device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
        D3D_FEATURE_LEVEL feature_levels[] =
        {
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
            D3D_FEATURE_LEVEL_9_3,
            D3D_FEATURE_LEVEL_9_2,
            D3D_FEATURE_LEVEL_9_1,
        };

        // スワップチェーンを作成するための設定オプション
        DXGI_SWAP_CHAIN_DESC swap_chain_desc;
        {
            swap_chain_desc.BufferDesc.Width = screen_width;
            swap_chain_desc.BufferDesc.Height = screen_height;
            swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60;
            swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
            swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
            swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

            swap_chain_desc.SampleDesc.Count = 1;
            swap_chain_desc.SampleDesc.Quality = 0;
            swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            swap_chain_desc.BufferCount = 1;
            swap_chain_desc.OutputWindow = hwnd;
            swap_chain_desc.Windowed = TRUE;
            swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
            swap_chain_desc.Flags = 0;
        }

        D3D_FEATURE_LEVEL feature_level;

        // デバイス＆スワップチェーンの生成
        hr = D3D11CreateDeviceAndSwapChain(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            create_device_flags,
            feature_levels,
            ARRAYSIZE(feature_levels),
            D3D11_SDK_VERSION,
            &swap_chain_desc,
            &swap_chain,
            &device,
            &feature_level,
            &immediate_context
        );
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }

    // レンダーターゲットビューの生成
    {
        ID3D11Texture2D* back_buffer;
        hr = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&back_buffer));
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        hr = device->CreateRenderTargetView(back_buffer, nullptr, &render_target_view);
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        back_buffer->Release();
    }

    // 深度ステンシルビューの生成
    {
        D3D11_TEXTURE2D_DESC depth_stencil_buffer_desc;
        depth_stencil_buffer_desc.Width = screen_width;
        depth_stencil_buffer_desc.Height = screen_height;
        depth_stencil_buffer_desc.MipLevels = 1;
        depth_stencil_buffer_desc.ArraySize = 1;
        depth_stencil_buffer_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depth_stencil_buffer_desc.SampleDesc.Count = 1;
        depth_stencil_buffer_desc.SampleDesc.Quality = 0;
        depth_stencil_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
        depth_stencil_buffer_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        depth_stencil_buffer_desc.CPUAccessFlags = 0;
        depth_stencil_buffer_desc.MiscFlags = 0;
        hr = device->CreateTexture2D(&depth_stencil_buffer_desc, nullptr, &depth_stencil_buffer);
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        hr = device->CreateDepthStencilView(depth_stencil_buffer, nullptr, &depth_stencil_view);
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }

    // ビューポートの設定
    {
        D3D11_VIEWPORT viewport;
        viewport.TopLeftX = 0;
        viewport.TopLeftY = 0;
        viewport.Width = static_cast<float>(screen_width);
        viewport.Height = static_cast<float>(screen_height);
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        immediate_context->RSSetViewports(1, &viewport);
    }

    sprites[0] = new sprite(device);

    return true;
}

void framework::finalize()
{
    // 解放処理
    for (sprite*& sprite : sprites)
    {
        if (sprite != nullptr)
        {
            delete sprite;
            sprite = nullptr;
        }
    }

    if (depth_stencil_view != nullptr)
    {
        depth_stencil_view->Release();
        depth_stencil_view = nullptr;
    }
    if (depth_stencil_buffer != nullptr)
    {
        depth_stencil_buffer->Release();
        depth_stencil_buffer = nullptr;
    }
    if (render_target_view != nullptr)
    {
        render_target_view->Release();
        render_target_view = nullptr;
    }
    if (swap_chain != nullptr)
    {
        swap_chain->Release();
        swap_chain = nullptr;
    }
    if (immediate_context != nullptr)
    {
        immediate_context->Release();
        immediate_context = nullptr;
    }
    if (device != nullptr)
    {
        device->Release();
        device = nullptr;
    }
}

void framework::update(float elapsed_time/*Elapsed seconds from last frame*/)
{

}
void framework::render(float elapsed_time/*Elapsed seconds from last frame*/)
{
    HRESULT hr = S_OK;

    FLOAT color[] = { 0.2f, 0.5f, 0.5f, 1.0f };
    immediate_context->ClearRenderTargetView(render_target_view, color);
    immediate_context->ClearDepthStencilView(depth_stencil_view, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    immediate_context->OMSetRenderTargets(1, &render_target_view, depth_stencil_view);

    static float angle = 0;
    angle += 6.0f * elapsed_time;

    sprites[0]->render(immediate_context, 200, 200, 200, 200, angle, 1, 0, 0, 1);

    swap_chain->Present(0, 0);
}

