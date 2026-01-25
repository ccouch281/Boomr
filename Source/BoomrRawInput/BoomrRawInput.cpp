#include "BoomrRawInput.h"
#include "RawInputReader.h"

#if PLATFORM_WINDOWS
#include "Windows/WindowsApplication.h"
#include "Framework/Application/SlateApplication.h"
#endif

#define LOCTEXT_NAMESPACE "FBoomrRawInputModule"

void FBoomrRawInputModule::StartupModule()
{
#if PLATFORM_WINDOWS
	FRawInputReader::RegisterRawInput();

	RawInputHandler = MakeShareable(new FRawInputMessageHandler());

	if (FSlateApplication::IsInitialized())
	{
		TSharedPtr<GenericApplication> GenericApp = FSlateApplication::Get().GetPlatformApplication();
		if (GenericApp.IsValid())
		{
			FWindowsApplication* WinApp = static_cast<FWindowsApplication*>(GenericApp.Get());
			WinApp->AddMessageHandler(*RawInputHandler);
		}
	}
#endif
}

void FBoomrRawInputModule::ShutdownModule()
{
#if PLATFORM_WINDOWS
	if (RawInputHandler.IsValid() && FSlateApplication::IsInitialized())
	{
		TSharedPtr<GenericApplication> GenericApp = FSlateApplication::Get().GetPlatformApplication();
		if (GenericApp.IsValid())
		{
			FWindowsApplication* WinApp = static_cast<FWindowsApplication*>(GenericApp.Get());
			WinApp->RemoveMessageHandler(*RawInputHandler);
		}
	}
	RawInputHandler.Reset();
#endif
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FBoomrRawInputModule, BoomrRawInput)
