#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlinkingSphere.generated.h"

UCLASS()
class BOOMR_API ABlinkingSphere : public AActor
{
    GENERATED_BODY()
    
public:    
    ABlinkingSphere();

protected:
    virtual void BeginPlay() override;

public:    
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "Movement")
    void Launch(FVector Velocity);

    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UStaticMeshComponent* MeshComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    bool bEnableFloating = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float Amplitude = 200.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float Frequency = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    FVector MovementDirection = FVector(0, 1, 0);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visuals")
    float BlinkDuration = 0.15f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visuals")
    FLinearColor BlinkColor = FLinearColor::Red;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visuals")
    FName ColorParameterName = "Color";

private:
    FVector InitialLocation;
    float RunningTime;
    float BlinkTimer;
    bool bIsBlinking;
    FLinearColor OriginalColor;
    
    UPROPERTY()
    class UMaterialInstanceDynamic* DynamicMaterial;

    void SetMeshColor(FLinearColor Color);
};
