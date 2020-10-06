#pragma once

#include <windows.h>
#include <tchar.h>
#include <sstream>
#include <memory>

#include "misc.h"
#include "high_resolution_timer.h"

#include "imgui.h"

#include <d3d11.h>
#include <wrl.h>

#include "sprite.h"

#include "geometric_primitive.h"

class framework
{
public:
	const HWND hwnd;

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swap_chain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_view;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depth_stencil_view;

	std::unique_ptr<sprite> sprites[1024];

	std::unique_ptr<sprite> particle;
	std::unique_ptr<sprite> font;

	std::unique_ptr<sprite_batch> particle_batch;

	std::unique_ptr<geometric_primitive> cube;
	std::unique_ptr<geometric_primitive> cylinder;
	std::unique_ptr<geometric_primitive> sphere;

	framework(HWND hwnd) : hwnd(hwnd)
	{

	}
	~framework()
	{

	}
	int run()
	{
		MSG msg = {};

		if (!initialize()) return 0;
		while (WM_QUIT != msg.message)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				timer.tick();
				calculate_frame_stats();
				update(timer.time_interval());
				render(timer.time_interval());
			}
		}

#ifdef USE_IMGUI
		// imguiの解放処理	
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
#endif

		return static_cast<int>(msg.wParam);
	}

	LRESULT CALLBACK handle_message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
#ifdef USE_IMGUI
		// マウス操作設定
		if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) { return true; }
#endif
		switch (msg)
		{
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc;
			hdc = BeginPaint(hwnd, &ps);
			EndPaint(hwnd, &ps);
			break;
		}
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_CREATE:
			break;
		case WM_KEYDOWN:
			if (wparam == VK_ESCAPE) PostMessage(hwnd, WM_CLOSE, 0, 0);
			break;
		case WM_ENTERSIZEMOVE:
			timer.stop();
			break;

			timer.start();
			break;
		default:
			return DefWindowProc(hwnd, msg, wparam, lparam);
		}
		return 0;
	}

private:
	bool initialize();
	void update(float elapsed_time/*Elapsed seconds from last frame*/);
	void render(float elapsed_time/*Elapsed seconds from last frame*/);

private:
	high_resolution_timer timer;
	void calculate_frame_stats()
	{
		static int frames = 0;
		static float time_tlapsed = 0.0f;

		frames++;

		// 1秒あたりの平均フレーム
		if ((timer.time_stamp() - time_tlapsed) >= 1.0f)
		{
			float fps = static_cast<float>(frames);
			float mspf = 1000.0f / fps;
			std::ostringstream outs;
			outs.precision(6);
			outs << "FPS : " << fps << " / " << "Frame Time : " << mspf << " (ms)";
			SetWindowTextA(hwnd, outs.str().c_str());

			// リセット
			frames = 0;
			time_tlapsed += 1.0f;
		}
	}
};

