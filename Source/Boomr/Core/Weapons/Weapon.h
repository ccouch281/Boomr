#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponStatic.h"
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
    FImpactDecalData ImpactDecalData;

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
