#include "WeaponStatic.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "Components/DecalComponent.h"

void UWeaponStatic::FireHitscan(
	UWorld* World,
	AActor* Instigator,
	AController* InstigatorController,
	const FVector& Start,
	const FVector& Direction,
	float Range,
	float Damage,
	float ImpulseStrength
)
{
	if (!World) return;

	FVector End = Start + Direction * Range;
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(Instigator);

	TArray<FHitResult> HitResults;

	bool bHit = World->LineTraceMultiByChannel(
		HitResults,
		Start,
		End,
		ECC_Visibility,
		TraceParams
	);

	if (bHit)
	{
		for (int32 i = 0; i < HitResults.Num(); i++)
		{
			FHitResult& Hit = HitResults[i];

			if (i == 0)
			{
				DrawDebugLine(World, Start, Hit.ImpactPoint, FColor::Blue, false, 2.f);
				DrawDebugLine(World, Hit.ImpactPoint, End, FColor::Red, false, 2.f);
			}

			DrawDebugPoint(World, Hit.ImpactPoint, 20.f, FColor::Yellow, false, 2.f);

			AActor* HitActor = Hit.GetActor();
			if (HitActor)
			{
				UGameplayStatics::ApplyPointDamage(
					HitActor,
					Damage,
					Direction,
					Hit,
					InstigatorController,
					Instigator,
					UDamageType::StaticClass()
				);

				UPrimitiveComponent* HitComponent = Hit.GetComponent();
				if (HitComponent && HitComponent->IsSimulatingPhysics())
				{
					HitComponent->AddImpulseAtLocation(Direction * ImpulseStrength, Hit.ImpactPoint);
				}
			}
		}
	}
}

void UWeaponStatic::SpawnImpactDecal(UWorld* World, const FImpactDecalData& DecalData, const FHitResult& Hit)
{
	if (!World || DecalData.ImpactDecal.IsNull()) return;

	UMaterialInterface* DecalMaterial = DecalData.ImpactDecal.Get();
	if (!DecalMaterial)
	{
		DecalMaterial = DecalData.ImpactDecal.LoadSynchronous();
	}

	if (DecalMaterial)
	{
		UDecalComponent* ImpactDecalComponent = UGameplayStatics::SpawnDecalAttached
		(
			DecalMaterial,
			DecalData.ImpactDecalSize,
			Hit.GetComponent(),
			NAME_None,
			Hit.ImpactPoint,
			Hit.Normal.Rotation(),
			EAttachLocation::KeepWorldPosition,
			DecalData.ImpactDecalLifespan
		);
		if (ImpactDecalComponent)
		{
			ImpactDecalComponent->SetFadeScreenSize(0.f);
		}
	}
}
