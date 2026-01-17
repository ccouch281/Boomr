#pragma once

#include "CoreMinimal.h"

class FBooShaders : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
