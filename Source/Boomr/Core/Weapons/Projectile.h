// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Engine/EngineTypes.h"
#include "Projectile.generated.h"

class UDamageType;

UCLASS(meta = (ShortTooltip = "A projectile can be launched and do damage. Bullets, rockets, grenades, etc. "))
class BOOMR_API AProjectile : public AActor
{
	GENERATED_BODY()

public:
	AProjectile();
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	virtual void SetOwner(AActor *NewOwner) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USphereComponent* SphereCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	URadialForceComponent* RadialForce;


	UFUNCTION(BlueprintCallable, Category = Projectile)
	void LaunchSelf();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	bool bLaunchOnBeginPlay;

    // Damage system (child classes decide which to use)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Damage)
    float Damage = 50.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Damage)
    TSubclassOf<UDamageType> DamageType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Damage)
    float DamageRadius = 300.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Damage)
    bool bDoFullDamage = false;

    UFUNCTION(BlueprintCallable, Category = Damage)
    void ApplyPointDamage(const FHitResult& Hit, float DamageAmount = -1.f);

    UFUNCTION(BlueprintCallable, Category = Damage)
    void ApplyRadialDamage(const FHitResult& Hit);

    // Radial damage params
    UFUNCTION(BlueprintCallable, Category = Damage)
    void SetRadialDamageParams(const FRadialDamageParams& Params);

    UFUNCTION(BlueprintCallable, Category = Damage)
    const FRadialDamageParams& GetRadialDamageParams() const;


	UFUNCTION()
	virtual void OnProjectileStop(const FHitResult& ImpactResult);

	UFUNCTION()
	virtual void OnProjectileBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity);

	UFUNCTION()
	void OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    virtual void Explode(const FHitResult& Hit);

protected:
	FPointDamageEvent PointDamageEvent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Damage)
	FRadialDamageParams RadialDamageParams;



};
