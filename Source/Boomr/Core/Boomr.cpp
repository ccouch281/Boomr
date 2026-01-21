// Fill out your copyright notice in the Description page of Project Settings.

#include "Boomr.h"
#include "Modules/ModuleManager.h"
#include "HAL/IConsoleManager.h"

void FBoomrModule::StartupModule()
{
	FDefaultGameModuleImpl::StartupModule();

	// Disable Eye Adaptation
	if (IConsoleVariable* CVarEyeAdapt = IConsoleManager::Get().FindConsoleVariable(TEXT("r.EyeAdaptationQuality")))
	{
		CVarEyeAdapt->Set(0, ECVF_SetByCode);
	}

	// Disable Tonemapper (sets quality to 0, which is basic/off)
	if (IConsoleVariable* CVarTonemapper = IConsoleManager::Get().FindConsoleVariable(TEXT("r.Tonemapper.Quality")))
	{
		CVarTonemapper->Set(0, ECVF_SetByCode);
	}
}

IMPLEMENT_PRIMARY_GAME_MODULE( FBoomrModule, Boomr, "Boomr" );
