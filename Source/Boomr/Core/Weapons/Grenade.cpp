// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Grenade.h"
#include "Kismet/GameplayStatics.h"

AGrenade::AGrenade()
{
	ProjectileMovement->InitialSpeed = 2500.f;
	ProjectileMovement->MaxSpeed = 2800.f;
	ProjectileMovement->ProjectileGravityScale = 0.75f;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->Bounciness = 0.3f;
	ProjectileMovement->Friction = 0.8f;
	ProjectileMovement->MinFrictionFraction = 0.14f;

	FuseTime = 0.4f;
	BounceSound = nullptr;
}

void AGrenade::OnProjectileStop(const FHitResult& ImpactResult)
{
	Super::OnProjectileStop(ImpactResult);
	GetWorld()->GetTimerManager().SetTimer
	(
		TimerHandle_ExplosionFuse, 
		[=]()
		{ 
			Explode(ImpactResult); 
		}, 
		FuseTime, 
		false
	);
}

void AGrenade::OnProjectileBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	Super::OnProjectileBounce(ImpactResult, ImpactVelocity);
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), BounceSound, GetActorLocation());
}
