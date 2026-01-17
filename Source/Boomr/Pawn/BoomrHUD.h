#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BoomrHUD.generated.h"

UCLASS()
class BOOMR_API ABoomrHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;
	virtual void BeginPlay() override;

private:
	void DrawFPS();
	void DrawCrosshair();
};
