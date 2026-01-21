// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Projectile.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Actor.h"
#include "GameFramework/DamageType.h"

AProjectile::AProjectile()
{
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->SetSphereRadius(8.f);
	SphereCollision->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	RootComponent = SphereCollision;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 3200.f;
	ProjectileMovement->MaxSpeed = 3200.f;
	ProjectileMovement->ProjectileGravityScale = 0.0f;
	ProjectileMovement->bSimulationEnabled = false;	// disabled simulation so I can control when the projectile gets launched

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->SetCollisionProfileName(TEXT("NoCollision"));
	ProjectileMesh->SetupAttachment(RootComponent);

	RadialForce = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForce"));
	RadialForce->Radius = 1000.f;
	RadialForce->ImpulseStrength = 800.f;
	RadialForce->bImpulseVelChange = true;
	RadialForce->SetupAttachment(RootComponent);

	InitialLifeSpan = 15.f;
	bLaunchOnBeginPlay = true;
}

void AProjectile::SetRadialDamageParams(const FRadialDamageParams& Params)
{
    RadialDamageParams = Params;

    // Inform radial force and projectile properties
    RadialForce->Radius = RadialDamageParams.OuterRadius;
    DamageRadius = RadialDamageParams.OuterRadius;

    // Use base damage as Damage if provided
    Damage = RadialDamageParams.BaseDamage;

    // Map inner/outer radius maybe to impulse strength heuristically
    float Inner = RadialDamageParams.InnerRadius;
    float Outer = RadialDamageParams.OuterRadius;
    if (Outer > 0.f)
    {
        // Impulse strength scaled by ratio
        RadialForce->ImpulseStrength = FMath::Clamp(800.f * (Outer / 300.f), 0.f, 5000.f);
    }
}

const FRadialDamageParams& AProjectile::GetRadialDamageParams() const
{
    return RadialDamageParams;
}

void AProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();


	// bind delegates
	SphereCollision->OnComponentHit.AddDynamic(this, &AProjectile::OnComponentHit);
	ProjectileMovement->OnProjectileStop.AddDynamic(this, &AProjectile::OnProjectileStop);
	ProjectileMovement->OnProjectileBounce.AddDynamic(this, &AProjectile::OnProjectileBounce);

}

void AProjectile::SetOwner(AActor *NewOwner) 
{
	Super::SetOwner(NewOwner);
	if (NewOwner )
	{
        SphereCollision->IgnoreActorWhenMoving(NewOwner, true);
		if (auto OwnerPrimitive = Cast<UPrimitiveComponent>(NewOwner->GetRootComponent()))
		{
			SphereCollision->IgnoreComponentWhenMoving(OwnerPrimitive, true);
			OwnerPrimitive->IgnoreComponentWhenMoving(SphereCollision, true);
			OwnerPrimitive->IgnoreActorWhenMoving(this, true);
		}
	}
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (bLaunchOnBeginPlay)
	{
		LaunchSelf();
	}
}

void AProjectile::LaunchSelf()
{
	ProjectileMovement->bSimulationEnabled = true;
	ProjectileMovement->SetUpdatedComponent(SphereCollision); // This is neccessary if you want to re-launch after the projectile rolls to a stop
	ProjectileMovement->Velocity = (GetActorForwardVector() * ProjectileMovement->InitialSpeed);
}

void AProjectile::OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    // Default behavior: apply point damage to the other actor (if valid) and destroy self.
    if (!OtherActor || OtherActor == this || OtherActor == GetOwner() || ProjectileMovement->bShouldBounce)
    {
        return;
    }

    ApplyPointDamage(Hit);

    // Optionally explode instead for actors that should cause an explosion - child classes can override this
    Destroy();
}

void AProjectile::OnProjectileStop(const FHitResult& ImpactResult)
{
}

void AProjectile::OnProjectileBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
}

void AProjectile::Explode(const FHitResult& Hit)
{
    RadialForce->FireImpulse();

    auto CenterOfExplosion = Hit.ImpactPoint;
    DrawDebugSphere(GetWorld(), CenterOfExplosion, RadialForce->Radius, 12, FColor::Green, false, 1.0f, 0U, 2.f);
    DrawDebugSphere(GetWorld(), CenterOfExplosion, 500.f, 12, FColor::Red, false, 1.0f, 0U, 2.f);

    // Apply radial damage using Unreal's damage system
    ApplyRadialDamage(Hit);

    Destroy();
}

void AProjectile::ApplyPointDamage(const FHitResult& Hit, float DamageAmount)
{
    if (DamageAmount <= 0.f)
    {
        DamageAmount = Damage;
    }

    AActor* HitActor = Hit.GetActor();
    if (!HitActor) return;

    // Try to find a controller to pass as the instigator; use owner if possible
    AController* InstigatorController = nullptr;
    if (APawn* OwnerPawn = Cast<APawn>(GetOwner()))
    {
        InstigatorController = OwnerPawn->GetController();
    }

    // Use GameplayStatics to apply point damage so OnTakePointDamage delegates are triggered
    FVector ShotDir = ProjectileMovement ? -ProjectileMovement->Velocity.GetSafeNormal() : -GetActorForwardVector();
    UGameplayStatics::ApplyPointDamage(HitActor, DamageAmount, ShotDir, Hit, InstigatorController, this, DamageType ? DamageType : UDamageType::StaticClass());
}

void AProjectile::ApplyRadialDamage(const FHitResult& Hit)
{
    FVector Origin = Hit.ImpactPoint;

    // Try to find a controller to pass as the instigator; use owner if possible
    AController* InstigatorController = nullptr;
    if (APawn* OwnerPawn = Cast<APawn>(GetOwner()))
    {
        InstigatorController = OwnerPawn->GetController();
    }

    UGameplayStatics::ApplyRadialDamage(
        GetWorld(),
        Damage,
        Origin,
        DamageRadius,
        DamageType ? DamageType : UDamageType::StaticClass(),
        TArray<AActor*>(),
        this,
        InstigatorController,
        bDoFullDamage
    );
}


