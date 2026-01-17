#pragma once
#include "CoreMinimal.h"
#include "Weapon.h"
#include "Pistol.generated.h"

UCLASS()
class APistol : public AWeapon
{
	GENERATED_BODY()
public:
	APistol();

	virtual void Shoot() override;
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Weapon|Recoil")
	FVector RecoilOffset = FVector(-5.0f, 0.0f, 0.0f);

	UPROPERTY(EditAnywhere, Category = "Weapon|Recoil")
	FRotator RecoilRotation = FRotator(-3.0f, 0.0f, 0.0f);

	UPROPERTY(EditAnywhere, Category = "Weapon|Recoil")
	float RecoilDuration = 0.1f;

	UPROPERTY(EditAnywhere, Category = "Weapon|Recoil")
	float RecoilRecoverySpeed = 10.0f;

private:
	FVector OriginalLocation;
	FRotator OriginalRotation;
	float RecoilTimer;
	bool bIsRecoiling;

	void StartRecoil();
};
