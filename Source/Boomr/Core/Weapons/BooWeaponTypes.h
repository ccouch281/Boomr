#pragma once

#include "CoreMinimal.h"
#include "BooWeaponTypes.generated.h"

UENUM(BlueprintType)
enum class EBooFireMode : uint8
{
	Hitscan,
	Projectile,
	Throwable
};

UENUM(BlueprintType)
enum class EBooTriggerType : uint8
{
	SemiAuto,
	Automatic,
	Charged
};

USTRUCT(BlueprintType)
struct FBooWeaponVisuals
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Visuals")
	class USkeletalMesh* WeaponMesh;

	UPROPERTY(EditAnywhere, Category = "Visuals")
	class UParticleSystem* MuzzleFlash;

	UPROPERTY(EditAnywhere, Category = "Visuals")
	class USoundBase* FireSound;
};
