#include "Weapon.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Engine/SkeletalMesh.h"

AWeapon::AWeapon()
{
    PrimaryActorTick.bCanEverTick = false;

    WeaponRoot = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponRoot"));
    SetRootComponent(WeaponRoot);

    FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
    FirstPersonMesh->SetupAttachment(WeaponRoot);

    ThirdPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ThirdPersonMesh"));
    ThirdPersonMesh->SetupAttachment(WeaponRoot);
}

void AWeapon::BeginPlay()
{
    Super::BeginPlay();

    // Load assets via Asset Manager
    FStreamableManager& Streamable = UAssetManager::GetStreamableManager();

    if (!FirstPersonMeshAsset.IsNull())
    {
        Streamable.RequestAsyncLoad(FirstPersonMeshAsset.ToSoftObjectPath(), FStreamableDelegate::CreateUObject(this, &AWeapon::OnFirstPersonMeshLoaded));
    }

    if (!ThirdPersonMeshAsset.IsNull())
    {
        Streamable.RequestAsyncLoad(ThirdPersonMeshAsset.ToSoftObjectPath(), FStreamableDelegate::CreateUObject(this, &AWeapon::OnThirdPersonMeshLoaded));
    }

    // Pre-load visual/sound assets so they are ready when shooting
    if (!ImpactDecalData.ImpactDecal.IsNull())
    {
        Streamable.RequestAsyncLoad(ImpactDecalData.ImpactDecal.ToSoftObjectPath());
    }
    if (!PrimarySound.IsNull())
    {
        Streamable.RequestAsyncLoad(PrimarySound.ToSoftObjectPath());
    }
}

void AWeapon::OnFirstPersonMeshLoaded()
{
    if (FirstPersonMeshAsset.IsValid())
    {
        FirstPersonMesh->SetSkeletalMesh(FirstPersonMeshAsset.Get());
    }
}

void AWeapon::OnThirdPersonMeshLoaded()
{
    if (ThirdPersonMeshAsset.IsValid())
    {
        ThirdPersonMesh->SetSkeletalMesh(ThirdPersonMeshAsset.Get());
    }
}

void AWeapon::Shoot()
{
}

void AWeapon::GetTraceStartAndRotation(FVector& OutStart, FRotator& OutRotation) const
{
    USceneComponent* AttachParent = GetRootComponent() ? GetRootComponent()->GetAttachParent() : nullptr;
    if (AttachParent)
    {
        OutStart = AttachParent->GetComponentLocation();
        OutRotation = AttachParent->GetComponentRotation();
    }
    else
    {
        AActor* MyOwner = GetOwner();
        if (MyOwner)
        {
            MyOwner->GetActorEyesViewPoint(OutStart, OutRotation);
        }
        else
        {
             OutStart = GetActorLocation();
             OutRotation = GetActorRotation();
        }
    }
}

