#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UCLASS()
class BOOMR_API AWeapon : public AActor
{
    GENERATED_BODY()

public:
    AWeapon();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Mesh")
    USceneComponent* WeaponRoot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Mesh")
    USkeletalMeshComponent* FirstPersonMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Mesh")
    USkeletalMeshComponent* ThirdPersonMesh;

    virtual void Shoot();
    
    void GetTraceStartAndRotation(FVector& OutStart, FRotator& OutRotation) const;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon|Visuals")
    TSoftObjectPtr<UMaterialInterface> ImpactDecal;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon|Visuals")
    FVector ImpactDecalSize = FVector(5.f, 10.f, 10.f);

    UPROPERTY(EditDefaultsOnly, Category = "Weapon|Visuals")
    float ImpactDecalLifespan = 10.f;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon|Sound")
    TSoftObjectPtr<USoundBase> PrimarySound;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon|Mesh")
    TSoftObjectPtr<USkeletalMesh> FirstPersonMeshAsset;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon|Mesh")
    TSoftObjectPtr<USkeletalMesh> ThirdPersonMeshAsset;

protected:
    virtual void BeginPlay() override;

    void OnFirstPersonMeshLoaded();
    void OnThirdPersonMeshLoaded();
};
