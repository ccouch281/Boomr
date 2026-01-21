// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "Grenade.generated.h"

UCLASS()
class BOOMR_API AGrenade : public AProjectile {
	GENERATED_BODY()

public:

	AGrenade();

	//virtual void PostInitializeComponents() override;

	
	virtual void OnProjectileStop(const FHitResult& ImpactResult) override;

	
	virtual void OnProjectileBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity) override;

	UPROPERTY(EditDefaultsOnly)
	USoundBase* BounceSound;

	FTimerHandle TimerHandle_ExplosionFuse;

	float FuseTime;
	
};
