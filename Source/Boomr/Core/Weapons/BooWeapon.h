#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BooWeaponData.h"
#include "BooWeapon.generated.h"

UCLASS()
class BOOMR_API ABooWeapon : public AActor
{
	GENERATED_BODY()

public:
	ABooWeapon();

	/** One-stop shop for firing input. */
	void StartPrimaryFire();
	void StopPrimaryFire();

	/** Set the weapon's configuration from a data asset */
	void InitializeWeapon(UBooWeaponData* NewData);

protected:
	virtual void Tick(float DeltaTime) override;

	/** The core execution point. Pulls 64-bit precision data and manifests the shot. */
	void ExecuteShot(float ChargeRatio = 1.0f);

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	UBooWeaponData* WeaponData;

private:
	// Trigger State
	bool bIsTriggerHeld = false;
	float LastFireTime = 0.0f;
	float CurrentChargeTime = 0.0f;
	bool bWantsToFire = false;

	// Helper to get PC for precision data
	class ABooPlayerController* GetPlayerController() const;
};
