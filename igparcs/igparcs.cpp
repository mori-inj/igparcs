#include <windows.h>
#include <gdiplus.h>
#include <time.h>
//#include "resource.h"
using namespace Gdiplus;
#pragma comment(lib, "gdiplus")

#define LD long double


HINSTANCE g_hInst;
HWND hWndMain;
LPCTSTR lpszClass = TEXT("GdiPlusStart");

void OnPaint(HDC hdc, int ID, int x, int y);
void OnPaintA(HDC hdc, int ID, int x, int y, double alpha);
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

class Particle
{
	private:
		int size;
		HPEN hPen;
	public:
		COLORREF color;
		int x,y;
		LD vx, vy;
		Particle()
		{
			this->x = rand()%80*10;
			this->y = rand()%600-60;
			vx = rand()%6-3;
			vy = 0;//rand()%2-1;
			while(vx==0 && vy ==0)
			{
				vx = rand()%6-3;
				vy = 0;//rand()%2-1;
			}
			size = 3;
			color = RGB(rand()%256, rand()%256, rand()%256);
			hPen = CreatePen(PS_SOLID, 1, color);
		}
		Particle(LD x, LD y)
		{
			this->x = x;
			this->y = y;
			vx = rand()%2-1;
			vy = rand()%2-1;
			while(vx==0 && vy ==0)
			{
				vx = rand()%2-1;
				vy = rand()%2-1;
			}
			size = 3;
			color = RGB(rand()%256, rand()%256, rand()%256);
			hPen = CreatePen(PS_SOLID, 1, color);
		}

		void draw(HDC hdc)
		{
			SelectObject(hdc, hPen);
			Ellipse(hdc, x-size, y-size, x+size, y+size);
		}
		void collision(Particle that)
		{
			if( (this->x - that.x)*(this->x - that.x) + (this->y - that.y)*(this->y - that.y) <= 64)
			{
				vx = -vx;
				vy = -vy;
				that.vx = -that.vx;
				that.vy = -that.vy;

				x = x + vx;
				y = y + vy;
				that.x = that.x + that.vx;
				that.y = that.y + that.vy;

			}
		}
		void move()
		{
			x = x + vx;
			y = y + vy;
			if(y<0)
			{
				y=0;
				vy = -vy;
			}
			else if(y>600-60)
			{
				y = 600-60;
				vy = -vy;
			}

			if(x<0)
			{
				x=0;
				vx = -vx;
			}
			else if(x>800)
			{
				x = 800;
				vx = -vx;
			}
		}
		//??
};


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
	HWND     hWnd;
	MSG		 msg;
	WNDCLASS WndClass;

	g_hInst = hInstance;

	ULONG_PTR gpToken;
	GdiplusStartupInput gpsi;
	if (GdiplusStartup(&gpToken, &gpsi, NULL) != Ok)
	{
		MessageBox(NULL, TEXT("GDI+ 라이브러리를 초기화할 수 없습니다."), TEXT("알림"), MB_OK);
		return 0;
	}


	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = L"Window Class Name";
	RegisterClass(&WndClass);
	hWnd = CreateWindow(
		L"Window Class Name",
		L"Window Title Name",
		WS_OVERLAPPEDWINDOW,
		GetSystemMetrics(SM_CXFULLSCREEN) / 2 - 400,
		GetSystemMetrics(SM_CYFULLSCREEN) / 2 - 300,
		800,
		600,
		NULL,
		NULL,
		hInstance,
		NULL
		);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc, MemDC;
	PAINTSTRUCT ps;

	HBRUSH hBrush;
	HBRUSH oldBrush;
	HPEN hPen;
	HPEN oldPen;

	HBITMAP hBit, OldBit;
	RECT crt;

	static Particle particle[50];


	switch (iMsg)
	{
	case WM_CREATE:
		srand((unsigned)time(NULL));
		SetTimer(hWnd, 1, 10, 0);
		break;

	case WM_TIMER:
		InvalidateRect(hWnd, NULL, FALSE);

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		GetClientRect(hWnd, &crt);

		MemDC = CreateCompatibleDC(hdc);
		hBit = CreateCompatibleBitmap(hdc, crt.right, crt.bottom);
		OldBit = (HBITMAP)SelectObject(MemDC, hBit);
		hBrush = CreateSolidBrush(RGB(0, 0, 0));
		oldBrush = (HBRUSH)SelectObject(MemDC, hBrush);
		hPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
		oldPen = (HPEN)SelectObject(MemDC, hPen);

		//FillRect(MemDC, &crt, hBrush);
		SetBkColor(MemDC, RGB(255, 255, 255));

		for(int i=0; i<50; i++)
			for(int j=i+1; j<50; j++)
				particle[i].collision(particle[j]);

		for(int i=0; i<50; i++)
		{
			particle[i].move();
			particle[i].draw(MemDC);
		}


		//OnPaint(MemDC, TITLE0, 0, 0);

		BitBlt(hdc, 0, 0, crt.right, crt.bottom, MemDC, 0, 0, SRCCOPY);
		SelectObject(MemDC, OldBit);
		DeleteDC(MemDC);
		SelectObject(MemDC, oldPen);
		DeleteObject(hPen);
		SelectObject(MemDC, oldBrush);
		DeleteObject(hBrush);
		DeleteObject(hBit);

		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, iMsg, wParam, lParam);
}

void OnPaint(HDC hdc, int ID, int x, int y)
{
	Graphics G(hdc);
	HRSRC hResource = FindResource(g_hInst, MAKEINTRESOURCE(ID), TEXT("PNG"));
	if (!hResource)
		return;

	DWORD imageSize = SizeofResource(g_hInst, hResource);
	HGLOBAL hGlobal = LoadResource(g_hInst, hResource);
	LPVOID pData = LockResource(hGlobal);

	HGLOBAL hBuffer = GlobalAlloc(GMEM_MOVEABLE, imageSize);
	LPVOID pBuffer = GlobalLock(hBuffer);
	CopyMemory(pBuffer, pData, imageSize);
	GlobalUnlock(hBuffer);

	IStream *pStream;
	HRESULT hr = CreateStreamOnHGlobal(hBuffer, TRUE, &pStream);

	Image I(pStream);
	pStream->Release();
	if (I.GetLastStatus() != Ok) return;

	G.DrawImage(&I, x, y, I.GetWidth(), I.GetHeight());
}


void OnPaintA(HDC hdc, int ID, int x, int y, double alpha)
{
	Graphics G(hdc);
	HRSRC hResource = FindResource(g_hInst, MAKEINTRESOURCE(ID), TEXT("PNG"));
	if (!hResource)
		return;

	ColorMatrix ClrMatrix =
	{
		1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, alpha, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 1.0f
	};

	ImageAttributes ImgAttr;
	ImgAttr.SetColorMatrix(&ClrMatrix, ColorMatrixFlagsDefault, ColorAdjustTypeBitmap);

	DWORD imageSize = SizeofResource(g_hInst, hResource);
	HGLOBAL hGlobal = LoadResource(g_hInst, hResource);
	LPVOID pData = LockResource(hGlobal);

	HGLOBAL hBuffer = GlobalAlloc(GMEM_MOVEABLE, imageSize);
	LPVOID pBuffer = GlobalLock(hBuffer);
	CopyMemory(pBuffer, pData, imageSize);
	GlobalUnlock(hBuffer);

	IStream *pStream;
	HRESULT hr = CreateStreamOnHGlobal(hBuffer, TRUE, &pStream);

	Image I(pStream);
	pStream->Release();
	if (I.GetLastStatus() != Ok) return;

	RectF destination(0, 0, I.GetWidth(), I.GetHeight());
	G.DrawImage(&I, destination, x, y, I.GetWidth(), I.GetHeight(), UnitPixel, &ImgAttr);
}