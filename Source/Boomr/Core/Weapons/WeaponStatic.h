#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WeaponStatic.generated.h"

USTRUCT(BlueprintType)
struct FImpactDecalData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Visuals")
	TSoftObjectPtr<UMaterialInterface> ImpactDecal;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Visuals")
	FVector ImpactDecalSize = FVector(5.f, 10.f, 10.f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Visuals")
	float ImpactDecalLifespan = 10.f;
};

UCLASS()
class BOOMR_API UWeaponStatic : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static void FireHitscan(
		UWorld* World,
		AActor* Instigator,
		AController* InstigatorController,
		const FVector& Start,
		const FVector& Direction,
		float Range = 10000.f,
		float Damage = 20.f,
		float ImpulseStrength = 500000.f
	);

	static void SpawnImpactDecal(UWorld* World, const FImpactDecalData& DecalData, const FHitResult& Hit);
};
