#include "BlinkingSphere.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

ABlinkingSphere::ABlinkingSphere()
{
    PrimaryActorTick.bCanEverTick = true;

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    RootComponent = MeshComponent;

    // Default values
    RunningTime = 0.0f;
    BlinkTimer = 0.0f;
    bIsBlinking = false;
    OriginalColor = FLinearColor::White;
}

void ABlinkingSphere::BeginPlay()
{
    Super::BeginPlay();
    InitialLocation = GetActorLocation();

    if (!MovementDirection.IsNearlyZero())
    {
        MovementDirection.Normalize();
    }

    // Create dynamic material instance
    UMaterialInterface* CurrentMaterial = MeshComponent->GetMaterial(0);
    if (CurrentMaterial)
    {
        DynamicMaterial = MeshComponent->CreateDynamicMaterialInstance(0, CurrentMaterial);
        if (DynamicMaterial)
        {
            // Try to get the original color if possible, but default to White
            // We'll just assume white for now or the user can set it.
            DynamicMaterial->GetVectorParameterValue(ColorParameterName, OriginalColor);
        }
    }
}

void ABlinkingSphere::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Floating Movement
    if (bEnableFloating)
    {
        RunningTime += DeltaTime;
        FVector NewLocation = InitialLocation + (MovementDirection * FMath::Sin(RunningTime * Frequency) * Amplitude);
        SetActorLocation(NewLocation);
    }

    // Blinking logic
    if (bIsBlinking)
    {
        BlinkTimer -= DeltaTime;
        if (BlinkTimer <= 0.0f)
        {
            bIsBlinking = false;
            SetMeshColor(OriginalColor);
        }
    }
}

void ABlinkingSphere::Launch(FVector Velocity)
{
    bEnableFloating = false;
    if (MeshComponent)
    {
        MeshComponent->SetSimulatePhysics(true);
        MeshComponent->SetPhysicsLinearVelocity(Velocity);
    }
}

float ABlinkingSphere::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    // Trigger blink
    bIsBlinking = true;
    BlinkTimer = BlinkDuration;
    SetMeshColor(BlinkColor);

    return ActualDamage;
}

void ABlinkingSphere::SetMeshColor(FLinearColor Color)
{
    if (DynamicMaterial)
    {
        DynamicMaterial->SetVectorParameterValue(ColorParameterName, Color);
    }
}
