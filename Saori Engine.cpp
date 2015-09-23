#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>
#include <math.h>

#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>

#pragma comment(lib, "d2d1")
#pragma comment(lib, "dwrite")

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

// https://msdn.microsoft.com/fr-fr/library/windows/apps/dd370994.aspx

namespace DirectX
{
	HWND hwnd;
	ID2D1Factory* pDirect2DFactory;
	ID2D1HwndRenderTarget* pRenderTarget;

	HRESULT CreateDeviceResources()
	{
		HRESULT hr = S_OK;

		if (!pRenderTarget)
		{
			RECT rc;
			GetClientRect(hwnd, &rc);

			D2D1_SIZE_U size = D2D1::SizeU(
				rc.right - rc.left,
				rc.bottom - rc.top
				);

			// Create a Direct2D render target.
			hr = pDirect2DFactory->CreateHwndRenderTarget(
				D2D1::RenderTargetProperties(),
				D2D1::HwndRenderTargetProperties(hwnd, size),
				&pRenderTarget
				);
		}

		return hr;
	}

	void DiscardDeviceResources()
	{
		pRenderTarget->Release();
	}

	void OnResize(UINT width, UINT height)
	{
		if (pRenderTarget)
		{
			// Note: This method can fail, but it's okay to ignore the
			// error here, because the error will be returned again
			// the next time EndDraw is called.
			pRenderTarget->Resize(D2D1::SizeU(width, height));
		}
	}
}

#include "Saori Engine.h"

namespace DirectX
{
	HRESULT OnRender()
	{
		HRESULT hr = S_OK;

		hr = CreateDeviceResources();

		if (SUCCEEDED(hr))
		{
			pRenderTarget->BeginDraw();
			pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
			pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

			Saori::Render(pRenderTarget);

			hr = pRenderTarget->EndDraw();
		}

		if (hr == D2DERR_RECREATE_TARGET)
		{
			hr = S_OK;
			DiscardDeviceResources();
		}


		return hr;
	}

	LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		LRESULT result = 0;

		bool bHandled = false;
		switch (message)
		{
		case WM_CREATE:
			
			result = 1;
			bHandled = true;
			break;

		case WM_SIZE:
		{
			UINT width = LOWORD(lParam);
			UINT height = HIWORD(lParam);
			OnResize(width, height);
		}
			result = 0;
			bHandled = true;
			break;

		case WM_DISPLAYCHANGE:
			InvalidateRect(hwnd, NULL, FALSE);
			result = 0;
			bHandled = true;
			break;

		case WM_PAINT:
			OnRender();
			ValidateRect(hwnd, NULL);
			result = 0;
			bHandled = true;
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			result = 1;
			bHandled = true;
			break;
		}

		if (!bHandled)
			result = DefWindowProc(hwnd, message, wParam, lParam);

		return result;
	}
}



int WINAPI WinMain(
	HINSTANCE /* hInstance */,
	HINSTANCE /* hPrevInstance */,
	LPSTR /* lpCmdLine */,
	int /* nCmdShow */
	)
{
	HeapSetInformation(0, HeapEnableTerminationOnCorruption, 0, 0);

	if (SUCCEEDED(CoInitialize(0)))
	{
		HRESULT hr;

		// Create a Direct2D factory.
		hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &DirectX::pDirect2DFactory);

		if (SUCCEEDED(hr))
		{
			// Register the window class.
			WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
			wcex.style = CS_HREDRAW | CS_VREDRAW;
			wcex.lpfnWndProc = DirectX::WndProc;
			wcex.cbClsExtra = 0;
			wcex.cbWndExtra = sizeof(LONG_PTR);
			wcex.hInstance = HINST_THISCOMPONENT;
			wcex.hbrBackground = NULL;
			wcex.lpszMenuName = NULL;
			wcex.hCursor = LoadCursor(NULL, IDI_APPLICATION);
			wcex.lpszClassName = L"D2DDirectXApp";

			RegisterClassEx(&wcex);

			// Because the CreateWindow function takes its size in pixels,
			// obtain the system DPI and use it to scale the window size.
			FLOAT dpiX, dpiY;

			// The factory returns the current system DPI. This is also the value it will use
			// to create its own windows.
			DirectX::pDirect2DFactory->GetDesktopDpi(&dpiX, &dpiY);

			// Create the window.
			DirectX::hwnd = CreateWindow(
				L"D2DDirectXApp",
				L"Direct2D Demo App",
				WS_OVERLAPPEDWINDOW,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				static_cast<UINT>(ceil(640.f * dpiX / 96.f)),
				static_cast<UINT>(ceil(480.f * dpiY / 96.f)),
				NULL,
				NULL,
				HINST_THISCOMPONENT,
				0
				);
			hr = DirectX::hwnd ? S_OK : E_FAIL;
			if (SUCCEEDED(hr))
			{
				ShowWindow(DirectX::hwnd, SW_SHOWNORMAL);
				UpdateWindow(DirectX::hwnd);
			}
		}

		MSG msg;

		while (GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		CoUninitialize();
	}

	return 0;
}
