#include "Pistol.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Components/DecalComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/DamageType.h"

APistol::APistol()
{
    PrimaryActorTick.bCanEverTick = true;
    
    FirstPersonMesh->SetRelativeLocation(FVector(19.0f, 4.0f, -29.000002f));
    FirstPersonMesh->SetRelativeRotation(FRotator(0.0f, 269.999329f, 356.399078f));
    
    OriginalLocation = FirstPersonMesh->GetRelativeLocation();
    OriginalRotation = FirstPersonMesh->GetRelativeRotation();
    RecoilTimer = 0.0f;
    bIsRecoiling = false;
}

void APistol::Shoot()
{
    AActor* MyOwner = GetOwner();

    FVector Start;
    FRotator Rot;
    GetTraceStartAndRotation(Start, Rot);

    FVector End = Start + (Rot.Vector() * 10000.f);

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);
    if (MyOwner)
    {
        Params.AddIgnoredActor(MyOwner);
    }

    GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);

    if (Hit.bBlockingHit)
    {
        if (Hit.GetActor())
        {
            UGameplayStatics::ApplyDamage(Hit.GetActor(), 10.f, MyOwner ? MyOwner->GetInstigatorController() : nullptr, this, UDamageType::StaticClass());
        }

                if (!ImpactDecalData.ImpactDecal.IsNull())
        {
            UWeaponStatic::SpawnImpactDecal(GetWorld(), ImpactDecalData, Hit);
        }
        }

        DrawDebugPoint(GetWorld(), Hit.ImpactPoint, 10.f, FColor::Green, false, 2.f);

    if (!PrimarySound.IsNull())
    {
        USoundBase* Sound = PrimarySound.Get();
        if (!Sound)
        {
            Sound = PrimarySound.LoadSynchronous();
        }

        if (Sound)
        {
            FVector SoundLoc = MyOwner ? MyOwner->GetActorLocation() : GetActorLocation();
            UGameplayStatics::PlaySoundAtLocation(this, Sound, SoundLoc);
        }
    }

    DrawDebugLine
    (
        GetWorld(),
        (Hit.TraceStart - FVector(0.f, 0.f, 10.f)),
        (Hit.bBlockingHit ? Hit.ImpactPoint : Hit.TraceEnd),
        FColor::Red,
        false,
        2.f
    );
    
    StartRecoil();
}

void APistol::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    if (bIsRecoiling)
    {
        RecoilTimer += DeltaTime;
        
        if (RecoilTimer <= RecoilDuration)
        {
            // Recoil phase - move to recoil position
            float Alpha = RecoilTimer / RecoilDuration;
            FVector NewLocation = FMath::Lerp(OriginalLocation, OriginalLocation + RecoilOffset, Alpha);
            FRotator NewRotation = FMath::Lerp(OriginalRotation, OriginalRotation + RecoilRotation, Alpha);
            
            FirstPersonMesh->SetRelativeLocation(NewLocation);
            FirstPersonMesh->SetRelativeRotation(NewRotation);
        }
        else
        {
            // Recovery phase - return to original position
            FVector CurrentLocation = FirstPersonMesh->GetRelativeLocation();
            FRotator CurrentRotation = FirstPersonMesh->GetRelativeRotation();
            
            FVector NewLocation = FMath::VInterpTo(CurrentLocation, OriginalLocation, DeltaTime, RecoilRecoverySpeed);
            FRotator NewRotation = FMath::RInterpTo(CurrentRotation, OriginalRotation, DeltaTime, RecoilRecoverySpeed);
            
            FirstPersonMesh->SetRelativeLocation(NewLocation);
            FirstPersonMesh->SetRelativeRotation(NewRotation);
            
            // Check if we're close enough to original position to stop
            if (FVector::Dist(NewLocation, OriginalLocation) < 0.01f)
            {
                FirstPersonMesh->SetRelativeLocation(OriginalLocation);
                FirstPersonMesh->SetRelativeRotation(OriginalRotation);
                bIsRecoiling = false;
                RecoilTimer = 0.0f;
            }
        }
    }
}

void APistol::StartRecoil()
{
    bIsRecoiling = true;
    RecoilTimer = 0.0f;
}

