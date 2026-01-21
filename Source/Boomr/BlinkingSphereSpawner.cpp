#include "BlinkingSphereSpawner.h"
#include "BlinkingSphere.h"
#include "TimerManager.h"
#include "Engine/World.h"

ABlinkingSphereSpawner::ABlinkingSphereSpawner()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ABlinkingSphereSpawner::BeginPlay()
{
    Super::BeginPlay();

    if (bSpawnOnBeginPlay)
    {
        StartSpawning();
    }
}

void ABlinkingSphereSpawner::StartSpawning()
{
    if (!SphereClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("BlinkingSphereSpawner: SphereClass is not set!"));
        return;
    }

    RemainingToSpawn = NumberToSpawn;
    CurrentPoolIndex = 0;
    
    if (SpawnInterval > 0.0f)
    {
        GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &ABlinkingSphereSpawner::SpawnSingleSphere, SpawnInterval, true);
    }
    else
    {
        for (int32 i = 0; i < NumberToSpawn; ++i)
        {
            SpawnSingleSphere();
        }
    }
}

void ABlinkingSphereSpawner::SpawnSingleSphere()
{
    if (!bContinuous && RemainingToSpawn <= 0 && SpawnedPool.Num() >= NumberToSpawn)
    {
        GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);
        return;
    }

    ABlinkingSphere* SphereToLaunch = nullptr;

    if (SpawnedPool.Num() < NumberToSpawn)
    {
        // Initial spawn phase
        FVector Location = GetActorLocation();
        FRotator Rotation = GetActorRotation();
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.Instigator = GetInstigator();

        SphereToLaunch = GetWorld()->SpawnActor<ABlinkingSphere>(SphereClass, Location, Rotation, SpawnParams);
        if (SphereToLaunch)
        {
            SpawnedPool.Add(SphereToLaunch);
        }
        RemainingToSpawn--;
    }
    else if (bContinuous)
    {
        // Recycling phase
        SphereToLaunch = SpawnedPool[CurrentPoolIndex];
        CurrentPoolIndex = (CurrentPoolIndex + 1) % SpawnedPool.Num();

        if (SphereToLaunch)
        {
            SphereToLaunch->SetActorLocation(GetActorLocation(), false, nullptr, ETeleportType::TeleportPhysics);
            SphereToLaunch->SetActorRotation(GetActorRotation(), ETeleportType::TeleportPhysics);
            
            // Reset physics velocity before launching again
            if (UStaticMeshComponent* Mesh = Cast<UStaticMeshComponent>(SphereToLaunch->GetRootComponent()))
            {
                Mesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
                Mesh->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
            }
        }
    }

    if (SphereToLaunch)
    {
        float ConeHalfAngleRad = FMath::DegreesToRadians(ConeHalfAngleDegrees);
        FVector LaunchDirection = FMath::VRandCone(FVector::UpVector, ConeHalfAngleRad);
        float LaunchSpeed = FMath::FRandRange(LaunchSpeedMin, LaunchSpeedMax);
        
        SphereToLaunch->Launch(LaunchDirection * LaunchSpeed);
    }

    if (!bContinuous && RemainingToSpawn <= 0 && SpawnedPool.Num() >= NumberToSpawn)
    {
        GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);
    }
}
