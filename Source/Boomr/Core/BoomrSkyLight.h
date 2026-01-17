#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoomrSkyLight.generated.h"

UCLASS()
class BOOMR_API ABoomrSkyLight : public AActor
{
	GENERATED_BODY()
	
public:	
	ABoomrSkyLight();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class USkyLightComponent* SkyLightComponent;
};
