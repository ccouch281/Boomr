#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BooWeaponTypes.h"
#include "BooWeaponData.generated.h"

UCLASS(BlueprintType)
class BOOMR_API UBooWeaponData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Logic")
	EBooFireMode FireMode = EBooFireMode::Hitscan;

	UPROPERTY(EditAnywhere, Category = "Logic")
	EBooTriggerType TriggerType = EBooTriggerType::SemiAuto;

	/** For Automatic: Time between shots. For Charged: Min/Max time. */
	UPROPERTY(EditAnywhere, Category = "Logic")
	float FireRate = 0.1f;

	UPROPERTY(EditAnywhere, Category = "Logic")
	float Damage = 20.0f;

	UPROPERTY(EditAnywhere, Category = "Logic")
	float Range = 10000.0f;

	/** For Projectiles/Throwables */
	UPROPERTY(EditAnywhere, Category = "Logic")
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Visuals")
	FBooWeaponVisuals Visuals;
};
