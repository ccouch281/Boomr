#include "RawInputReader.h"

#if PLATFORM_WINDOWS
#include "Windows/AllowWindowsPlatformTypes.h"
#include <Windows.h>
#include "Windows/HideWindowsPlatformTypes.h"
#endif

FRawInputReader::FRawInputReader()
{
}

FRawInputReader::~FRawInputReader()
{
}

bool FRawInputReader::RegisterRawInput()
{
#if PLATFORM_WINDOWS
	RAWINPUTDEVICE Rid;
	Rid.usUsagePage = 0x01; 
	Rid.usUsage = 0x02; 
	Rid.dwFlags = RIDEV_INPUTSINK; 
	Rid.hwndTarget = NULL;
	return (RegisterRawInputDevices(&Rid, 1, sizeof(Rid)) != 0);
#else
	return false;
#endif
}


