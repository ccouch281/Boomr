#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlinkingSphereSpawner.generated.h"

UCLASS()
class BOOMR_API ABlinkingSphereSpawner : public AActor
{
    GENERATED_BODY()
    
public:    
    ABlinkingSphereSpawner();

protected:
    virtual void BeginPlay() override;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    TSubclassOf<class ABlinkingSphere> SphereClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    int32 NumberToSpawn = 10;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    float SpawnInterval = 0.1f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    float LaunchSpeedMin = 1000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    float LaunchSpeedMax = 2000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    float ConeHalfAngleDegrees = 30.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    bool bContinuous = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    bool bSpawnOnBeginPlay = true;

    UFUNCTION(BlueprintCallable, Category = "Spawning")
    void StartSpawning();

private:
    void SpawnSingleSphere();
    
    int32 RemainingToSpawn = 0;
    int32 CurrentPoolIndex = 0;
    FTimerHandle SpawnTimerHandle;

    UPROPERTY()
    TArray<class ABlinkingSphere*> SpawnedPool;
};
