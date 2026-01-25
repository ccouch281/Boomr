#pragma once

#include "CoreMinimal.h"

// Retrieve raw mouse deltas using Windows Raw Input (WM_INPUT)
class BOOMRRAWINPUT_API FRawInputReader
{
public:
	FRawInputReader();
	~FRawInputReader();

	static bool RegisterRawInput();
};
