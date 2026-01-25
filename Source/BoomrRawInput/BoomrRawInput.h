#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "RawInputMessageHandler.h"

class BOOMRRAWINPUT_API FBoomrRawInputModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	FORCEINLINE TSharedPtr<FRawInputMessageHandler> GetRawInputHandler() const { return RawInputHandler; }

private:
#if PLATFORM_WINDOWS
	TSharedPtr<FRawInputMessageHandler> RawInputHandler;
#endif
};
