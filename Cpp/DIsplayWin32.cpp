#include "../Header/DisplayWin32.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	InputDevice* Input = reinterpret_cast<InputDevice*>(GetProp(hwnd, L"InputDevice"));

	switch (umessage)
	{
	case WM_KEYDOWN:
	{
		if (static_cast<unsigned int>(wparam) == 27) PostQuitMessage(0);
		return 0;
	}
	case WM_INPUT:
	{
		UINT dwSize = 0;
		GetRawInputData(reinterpret_cast<HRAWINPUT>(lparam), RID_INPUT, nullptr, &dwSize, sizeof(RAWINPUTHEADER));
		LPBYTE lpb = new BYTE[dwSize];
		if (lpb == nullptr) {
			return 0;
		}

		if (GetRawInputData((HRAWINPUT)lparam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
			OutputDebugString(TEXT("GetRawInputData does not return correct size !\n"));

		RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(lpb);

		if (raw->header.dwType == RIM_TYPEKEYBOARD)
		{
			//printf(" Kbd: make=%04i Flags:%04i Reserved:%04i ExtraInformation:%08i, msg=%04i VK=%i \n",
			//	raw->data.keyboard.MakeCode,
			//	raw->data.keyboard.Flags,
			//	raw->data.keyboard.Reserved,
			//	raw->data.keyboard.ExtraInformation,
			//	raw->data.keyboard.Message,
			//	raw->data.keyboard.VKey);

			Input->OnKeyDown({
				raw->data.keyboard.MakeCode,
				raw->data.keyboard.Flags,
				raw->data.keyboard.VKey,
				raw->data.keyboard.Message
				});
		}
		else if (raw->header.dwType == RIM_TYPEMOUSE)
		{
			//printf(" Mouse: X=%04d Y:%04d \n", raw->data.mouse.lLastX, raw->data.mouse.lLastY);
			Input->OnMouseMove({
				raw->data.mouse.usFlags,
				raw->data.mouse.usButtonFlags,
				static_cast<int>(raw->data.mouse.ulExtraInformation),
				static_cast<int>(raw->data.mouse.ulRawButtons),
				static_cast<short>(raw->data.mouse.usButtonData),
				raw->data.mouse.lLastX,
				raw->data.mouse.lLastY
				});
		}

		delete[] lpb;
		return DefWindowProc(hwnd, umessage, wparam, lparam);
	}
		
	default:
	{
		return DefWindowProc(hwnd, umessage, wparam, lparam);
	}
	}
}

DisplayWin32::DisplayWin32()
{
	InitWNDCLASSEX();
	RECT windowRect = { 0, 0, static_cast<LONG>(pWidth), static_cast<LONG>(pHeight) };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	auto dwStyle = WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_THICKFRAME;

	auto posX = (GetSystemMetrics(SM_CXSCREEN) - pWidth) / 2;
	auto posY = (GetSystemMetrics(SM_CYSCREEN) - pHeight) / 2;

	phWnd = CreateWindowEx(WS_EX_APPWINDOW, applicationName, applicationName,
		dwStyle,
		posX, posY,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr, nullptr, hInstance, nullptr);
	ShowWindow(phWnd, SW_SHOW);
	SetForegroundWindow(phWnd);
	SetFocus(phWnd);
	ShowCursor(true);

	pInput = new InputDevice(&phWnd);
}

DisplayWin32::DisplayWin32(int Width, int Height) : pHeight(Height), pWidth(Width)
{
	InitWNDCLASSEX();
	RECT windowRect = { 0, 0, static_cast<LONG>(pWidth), static_cast<LONG>(pHeight) };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	auto dwStyle = WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_THICKFRAME;

	auto posX = (GetSystemMetrics(SM_CXSCREEN) - pWidth) / 2;
	auto posY = (GetSystemMetrics(SM_CYSCREEN) - pHeight) / 2;

	phWnd = CreateWindowEx(WS_EX_APPWINDOW, applicationName, applicationName,
		dwStyle,
		posX, posY,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr, nullptr, hInstance, nullptr);
	ShowWindow(phWnd, SW_SHOW);
	SetForegroundWindow(phWnd);
	SetFocus(phWnd);
	ShowCursor(true);

	pInput = new InputDevice(&phWnd);
	SetProp(phWnd, L"InputDevice", reinterpret_cast<HANDLE>(pInput));
}

int DisplayWin32::GetHeight()
{
	return pHeight;
}

int DisplayWin32::GetWidth()
{
	return pWidth;
}

InputDevice* DisplayWin32::GetInputDevice()
{
	return pInput;
}

void DisplayWin32::Release()
{

}

void DisplayWin32::InitWNDCLASSEX()
{
	pWC.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	pWC.lpfnWndProc = WndProc;
	pWC.cbClsExtra = 0;
	pWC.cbWndExtra = 0;
	pWC.hInstance = hInstance;
	pWC.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
	pWC.hIconSm = pWC.hIcon;
	pWC.hCursor = LoadCursor(nullptr, IDC_ARROW);
	pWC.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	pWC.lpszMenuName = nullptr;
	pWC.lpszClassName = applicationName;
	pWC.cbSize = sizeof(WNDCLASSEX);
	RegisterClassEx(&pWC);
}

