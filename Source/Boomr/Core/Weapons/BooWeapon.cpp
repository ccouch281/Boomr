#include "BooWeapon.h"
#include "BooWeaponData.h"
#include "BooPlayerController.h"
#include "BooPawn.h"
#include "WeaponStatic.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

ABooWeapon::ABooWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABooWeapon::InitializeWeapon(UBooWeaponData* NewData)
{
	WeaponData = NewData;
	// Update visual mesh etc here
}

void ABooWeapon::StartPrimaryFire()
{
	bIsTriggerHeld = true;
	
	if (WeaponData && WeaponData->TriggerType == EBooTriggerType::SemiAuto)
	{
		float CurrentTime = GetWorld()->GetTimeSeconds();
		if (CurrentTime - LastFireTime >= WeaponData->FireRate)
		{
			ExecuteShot();
			LastFireTime = CurrentTime;
		}
	}
}

void ABooWeapon::StopPrimaryFire()
{
	if (bIsTriggerHeld && WeaponData && WeaponData->TriggerType == EBooTriggerType::Charged)
	{
		// Bow/Charged shot: Fire on release
		float ChargeRatio = FMath::Clamp(CurrentChargeTime / WeaponData->FireRate, 0.0f, 1.0f);
		ExecuteShot(ChargeRatio);
	}

	bIsTriggerHeld = false;
	CurrentChargeTime = 0.0f;
}

void ABooWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsTriggerHeld || !WeaponData) return;

	float CurrentTime = GetWorld()->GetTimeSeconds();

	if (WeaponData->TriggerType == EBooTriggerType::Automatic)
	{
		if (CurrentTime - LastFireTime >= WeaponData->FireRate)
		{
			ExecuteShot();
			LastFireTime = CurrentTime;
		}
	}
	else if (WeaponData->TriggerType == EBooTriggerType::Charged)
	{
		CurrentChargeTime += DeltaTime;
	}
}

void ABooWeapon::ExecuteShot(float ChargeRatio)
{
	if (!WeaponData) return;

	FVector Start, Direction;
	
	// Pipeline shortcut: Bypassing component hierarchy to get 64-bit absolute source
	if (ABooPlayerController* PC = GetPlayerController())
	{
		PC->GetHighPrecisionFireTransform(Start, Direction);
	}
	else
	{
		// Fallback for AI or non-possessed weapons
		Start = GetActorLocation();
		Direction = GetActorForwardVector();
	}

	switch (WeaponData->FireMode)
	{
		case EBooFireMode::Hitscan:
		{
			UWeaponStatic::FireHitscan(
				GetWorld(), 
				GetOwner(), 
				GetPlayerController(), 
				Start, 
				Direction, 
				WeaponData->Range, 
				WeaponData->Damage * ChargeRatio
			);
			break;
		}
		case EBooFireMode::Projectile:
		case EBooFireMode::Throwable:
		{
			if (WeaponData->ProjectileClass)
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = GetOwner();
				SpawnParams.Instigator = Cast<APawn>(GetOwner());
				
				FRotator SpawnRot = Direction.Rotation();
				GetWorld()->SpawnActor<AActor>(WeaponData->ProjectileClass, Start, SpawnRot, SpawnParams);
			}
			break;
		}
	}

	// Visual Feedback
	if (WeaponData->Visuals.FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, WeaponData->Visuals.FireSound, Start);
	}
}

ABooPlayerController* ABooWeapon::GetPlayerController() const
{
	if (APawn* OwnerPawn = Cast<APawn>(GetOwner()))
	{
		return Cast<ABooPlayerController>(OwnerPawn->GetController());
	}
	return nullptr;
}
