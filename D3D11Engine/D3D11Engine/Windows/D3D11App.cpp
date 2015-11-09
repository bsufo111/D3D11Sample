#include "D3D11App.h"
#include <WindowsX.h>
#include <sstream>


namespace
{
	// This is just used to forward Windows messages from a global window
	// procedure to our member function window procedure because we cannot
	// assign a member function to WNDCLASS::lpfnWndProc.
	D3D11App* gd3dApp = 0;
}

LRESULT CALLBACK
MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Forward hwnd on because we can get messages (e.g., WM_CREATE)
	// before CreateWindow returns, and thus before mhMainWnd is valid.
	return gd3dApp->MsgProc(hwnd, msg, wParam, lParam);
}


D3D11App::D3D11App(HINSTANCE hInstance)
	: mhAppInst(hInstance),
	mMainWndCaption(L"D3D11 Application"),
	mClientWidth(1024),
	mClientHeight(600),
	mAppPaused(false),
	mMinimized(false),
	mMaximized(false),
	mResizing(false)
{
	gd3dApp = this;
}

D3D11App::~D3D11App()
{

}

bool D3D11App::Init()
{
	if (!InitMainWindow())
		return false;

	if (!InitDirect3D())
		return false;
	InitResource();
	mTimer.Start();
	return true;
}

bool D3D11App::InitMainWindow()
{
	WNDCLASS wc;
	//CS_DBLCLKS ˫���¼�
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = mhAppInst;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"D3DWndClassName";

	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}

	// Compute window rectangle dimensions based on requested client area dimensions.
	RECT R = { 0, 0, mClientWidth, mClientHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	mhMainWnd = CreateWindowW(L"D3DWndClassName", mMainWndCaption.c_str(),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, mhAppInst, 0);
	if (!mhMainWnd)
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	ShowWindow(mhMainWnd, SW_SHOW);
	UpdateWindow(mhMainWnd);
	// �ļ���ק�ڶ���   DragAcceptFiles  
	DragAcceptFiles(mhMainWnd, TRUE);
	return true;
}

LRESULT D3D11App::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DROPFILES:
	{
		HDROP hDrop = (HDROP)wParam;
		UINT nFileNum = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0); // ��ק�ļ�����  
		char strFileName[MAX_PATH];
		for (uint32 i = 0; i < nFileNum; i++)
		{
			DragQueryFileA(hDrop, i, strFileName, MAX_PATH);//�����ҷ���ļ���  
		}
		DragFinish(hDrop);      //�ͷ�hDrop
		OnDragFinish(strFileName);
	}
	return 0;
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case  WM_MOUSEWHEEL:
	{
		short zDelta = (short)HIWORD(wParam);
		OnMouseWheel(zDelta, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	}
	case  WM_LBUTTONDBLCLK:
		OnLButtonDblClk(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return 0;
}

int D3D11App::Run()
{
	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		// If there are Window messages then process them.
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// Otherwise, do animation/game stuff.
		else
		{

			if (!mAppPaused)
			{
				static DWORD timeLoop = 0;
				const DWORD FRAME_INTERVAL = 15; //15 ����
				//DWORD timerNow = mTimer.GetTime();
				DWORD timerNow = timeGetTime();
				if (timerNow < timeLoop + FRAME_INTERVAL)
				{
					Sleep(timeLoop + FRAME_INTERVAL - timerNow);
				}
				else
				{
					mTimer.Tick();
					timeLoop = mTimer.GetTime();
					timeLoop = timeGetTime();
					CalculateFrameStats();
					UpdateScene(mTimer.TotalTime()*1000.0f, mTimer.DeltaTime()*1000.0f);
					DrawScene();
				}
			}
		}
	}
	return (int)msg.wParam;
}

bool D3D11App::InitDirect3D()
{

	return true;
}

void D3D11App::OnResize()
{
	printf("OnResize");
}

void D3D11App::CalculateFrameStats()
{
	// Code computes the average frames per second, and also the 
	// average time it takes to render one frame.  These stats 
	// are appended to the window caption bar.

	static int frameCnt = 0;
	static float timeElapsed = 0.0f;
	static float  currentTime = 0.0f;//��ǰʱ��
	static float  lastTime = 0.0f;//����ʱ��

	frameCnt++;
	currentTime = mTimer.GetTime() * 0.001f;
	// Compute averages over one second period.
	if ((currentTime - lastTime) >= 1.0f)
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1


		fps = (float)frameCnt / (currentTime - lastTime);//������1���ӵ�FPSֵ
		lastTime = currentTime; //����ǰʱ��currentTime��������ʱ��lastTime����Ϊ��һ��Ļ�׼ʱ��
		float mspf = 1000.0f / fps;
		std::wostringstream outs;
		outs.precision(6);
		outs << mMainWndCaption << L"    "
			<< L"FPS: " << fps << L"    "
			<< L"Frame Time: " << mspf << L" (ms)";
		SetWindowText(mhMainWnd, outs.str().c_str());

		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}

}

void D3D11App::OnMouseDown(WPARAM btnState, int x, int y)
{
	bMouseDown = true;
	if (btnState == 1)
	{
		mouseLast.bLeftDown = true;
	}
	else
	{
		mouseLast.bLeftDown = false;
	}
	mouseLast.X = x;
	mouseLast.Y = y;
}

void D3D11App::OnMouseUp(WPARAM btnState, int x, int y)
{
	bMouseDown = false;
	if (btnState == 1)
	{
		mouseLast.bLeftDown = true;
	}
	else
	{
		mouseLast.bLeftDown = false;
	}
	mouseLast.X = x;
	mouseLast.Y = y;
}

void D3D11App::OnMouseMove(WPARAM btnState, int x, int y)
{
	if (bMouseDown)
	{
		MousePos _tmp(x, y);
		x -= mouseLast.X;
		y -= mouseLast.Y;
		mouseLast.X = _tmp.X;
		mouseLast.Y = _tmp.Y;
		if (mouseLast.bLeftDown)
		{
			
		}
		else
		{

		}
	}
}
void D3D11App::OnMouseWheel(short zDelta, int x, int y)
{

}
