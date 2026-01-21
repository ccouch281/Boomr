#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BooPlayerController.generated.h"

UCLASS()
class BOOMR_API ABooPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ABooPlayerController();

protected:
	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;
};
