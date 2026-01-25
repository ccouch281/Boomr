#include "RawInputMessageHandler.h"
#include "Engine/Engine.h"

#if PLATFORM_WINDOWS
#include "Windows/AllowWindowsPlatformTypes.h"
#include <Windows.h>
#include "Windows/HideWindowsPlatformTypes.h"

bool FRawInputMessageHandler::ProcessMessage(HWND WindowHandle, uint32 Message, WPARAM WordParameter, LPARAM LongParameter, int32& OutResult)
{
	if (Message != WM_INPUT)
	{
		return false;
	}

	RAWINPUT RawInputBuffer;
	UINT Size = sizeof(RAWINPUT);

	if (GetRawInputData((HRAWINPUT)LongParameter, RID_INPUT, &RawInputBuffer, &Size, sizeof(RAWINPUTHEADER)) != (UINT)-1)
	{
		const RAWMOUSE& Mouse = RawInputBuffer.data.mouse;
		if (RawInputBuffer.header.dwType == RIM_TYPEMOUSE && (Mouse.usFlags & MOUSE_MOVE_ABSOLUTE) == 0)
		{
			// Thread-safe "Sub-tick" accumulation for high-polling mice (e.g. 8000Hz)
			FPlatformAtomics::InterlockedAdd(&MouseBufferX, (int64)Mouse.lLastX);
			FPlatformAtomics::InterlockedAdd(&MouseBufferY, (int64)Mouse.lLastY);
		}

		// Handle Scroll Wheel (Vertical and Horizontal)
		if (Mouse.usButtonFlags & RI_MOUSE_WHEEL)
		{
			FPlatformAtomics::InterlockedAdd(&ScrollBufferY, (int64)(int16)Mouse.usButtonData);
		}
		if (Mouse.usButtonFlags & RI_MOUSE_HWHEEL)
		{
			FPlatformAtomics::InterlockedAdd(&ScrollBufferX, (int64)(int16)Mouse.usButtonData);
		}

		// Sub-tick click capture
		if (Mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN)
		{
			FPlatformAtomics::InterlockedExchange(&ClickBufferX, MouseBufferX);
			FPlatformAtomics::InterlockedExchange(&ClickBufferY, MouseBufferY);
			FPlatformAtomics::InterlockedExchange(&bHasPendingClick, 1);
		}
	}

	return false;
}

FIntPoint64 FRawInputMessageHandler::GetTotalAccumulation() const
{
	return FIntPoint64(MouseBufferX, MouseBufferY);
}

FIntPoint64 FRawInputMessageHandler::GetScrollAccumulation() const
{
	return FIntPoint64(ScrollBufferX, ScrollBufferY);
}

void FRawInputMessageHandler::SetAccumulation(int64 NewX, int64 NewY)
{
	FPlatformAtomics::InterlockedExchange(&MouseBufferX, NewX);
	FPlatformAtomics::InterlockedExchange(&MouseBufferY, NewY);
}

void FRawInputMessageHandler::ResetScroll()
{
	FPlatformAtomics::InterlockedExchange(&ScrollBufferX, 0);
	FPlatformAtomics::InterlockedExchange(&ScrollBufferY, 0);
}

bool FRawInputMessageHandler::ConsumeClick(FIntPoint64& OutCounts)
{
	if (FPlatformAtomics::InterlockedExchange(&bHasPendingClick, 0) == 1)
	{
		OutCounts.X = ClickBufferX;
		OutCounts.Y = ClickBufferY;
		return true;
	}
	return false;
}
#endif
