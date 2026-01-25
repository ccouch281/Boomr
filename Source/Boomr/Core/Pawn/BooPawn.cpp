#include "BooPawn.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "BooPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "Weapons/BooWeapon.h"
#include "Weapons/BooWeaponData.h"


ABooPawn::ABooPawn()
{
    PrimaryActorTick.bCanEverTick = true;

	//capsule radius and height
	GetCapsuleComponent()->SetCapsuleRadius(58.f);
	GetCapsuleComponent()->SetCapsuleHalfHeight(88.f);

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(RootComponent);
    Camera->SetRelativeLocation(FVector(0.f, 0.f, 64.f));
    Camera->FieldOfView = 110.f;
    Camera->bConstrainAspectRatio = true;
    Camera->AspectRatio = 1.7777778f;
    
    // We disable these to prevent the low-precision float-Euler ControllerRotation from fighting our 64-bit pipeline
    Camera->bUsePawnControlRotation = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;

    bJumpButtonIsHeld = false;
    bCrouchButtonIsHeld = false;
    CharacterMovementProps = FCharacterMovementProperties();
    ApplyCharacterMovementProperties(CharacterMovementProps);
}

void ABooPawn::BeginPlay()
{
	Super::BeginPlay();

	// Spawn default weapon
	if (DefaultWeaponData)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = this;
		
		CurrentWeapon = GetWorld()->SpawnActor<ABooWeapon>(ABooWeapon::StaticClass(), GetActorLocation(), GetActorRotation(), SpawnParams);
		if (CurrentWeapon)
		{
			CurrentWeapon->InitializeWeapon(DefaultWeaponData);
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("GripPoint"));
		}
	}
}

void ABooPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Default bindings for movement and actions
	PlayerInputComponent->BindAxis("MoveForward", this, &ABooPawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveBack", this, &ABooPawn::MoveBack);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABooPawn::MoveRight);
	PlayerInputComponent->BindAxis("MoveLeft", this, &ABooPawn::MoveLeft);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ABooPawn::MyStartJumping);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ABooPawn::MyStopJumping);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ABooPawn::MyStartCrouching);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ABooPawn::MyStopCrouching);
	
	// Unified firing input
	PlayerInputComponent->BindAction("Shoot", IE_Pressed, this, &ABooPawn::StartFire);
	PlayerInputComponent->BindAction("Shoot", IE_Released, this, &ABooPawn::StopFire);

	// Grenades are now just another weapon/item type you can switch to

	// turn and look
	//PlayerInputComponent->BindAxis("Turn", this, &ABooPawn::AddControllerYawInput);
	//PlayerInputComponent->BindAxis("LookUp", this, &ABooPawn::AddControllerPitchInput);

}

void ABooPawn::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
	if (PrevMovementMode == MOVE_Falling && GetCharacterMovement()->MovementMode == MOVE_Walking && bJumpButtonIsHeld)
	{
		MyStartJumping();
	}
}

void ABooPawn::ApplyCharacterMovementProperties(const FCharacterMovementProperties& MovementProperties)
{
	UCharacterMovementComponent& MoveComponent = *GetCharacterMovement();
	MoveComponent.MaxWalkSpeed = MovementProperties.WalkingMaxSpeed;
	MoveComponent.MaxWalkSpeedCrouched = MovementProperties.CrouchWalkingMaxSpeed;
	MoveComponent.MaxAcceleration = MovementProperties.WalkingAcceleration;
	MoveComponent.BrakingDecelerationWalking = MovementProperties.WalkingDeceleration;
	MoveComponent.BrakingFrictionFactor = MovementProperties.BrakingFrictionFactor;
	MoveComponent.SetWalkableFloorAngle(MovementProperties.WalkableFloorAngle);
	MoveComponent.GroundFriction = MovementProperties.GroundFriction;
	MoveComponent.bCanWalkOffLedgesWhenCrouching = MovementProperties.bCanWalkOffLedgesWhenCrouching;
	MoveComponent.bMaintainHorizontalGroundVelocity = MovementProperties.bMaintainHorizontalGroundVelocity;
	MoveComponent.JumpZVelocity = MovementProperties.JumpZVelocity;
	MoveComponent.BrakingDecelerationFalling = MovementProperties.BrakingDecelerationFalling;
	MoveComponent.AirControl = MovementProperties.AirControl;
	MoveComponent.AirControlBoostMultiplier = MovementProperties.AirControlBoostMultiplier;
	MoveComponent.AirControlBoostVelocityThreshold = MovementProperties.AirControlBoostVelocityThreshold;
	MoveComponent.FallingLateralFriction = MovementProperties.FallingLateralFriction;
	MoveComponent.GravityScale = MovementProperties.Gravity;
	MoveComponent.NavAgentProps.bCanCrouch = MovementProperties.bCanCrouch;
}

void ABooPawn::MoveForward(float Value)
{
	if (Value != 0.f)
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ABooPawn::MoveRight(float Value)
{
	if (Value != 0.f)
	{
		AddMovementInput(GetActorRightVector(), Value);
	}
}
void ABooPawn::MoveBack(float Value)
{
	if (Value != 0.f)
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
}
void ABooPawn::MoveLeft(float Value)
{
	if (Value != 0.f)
	{
		AddMovementInput(GetActorRightVector(), Value);
	}
}
void ABooPawn::MyStartJumping()
{
	Jump();
	bJumpButtonIsHeld = true;
}

void ABooPawn::MyStopJumping()
{
	StopJumping();
	bJumpButtonIsHeld = false;
}
void ABooPawn::MyStartCrouching()
{
	Crouch();
	bCrouchButtonIsHeld = true;
}
void ABooPawn::MyStopCrouching()
{
	UnCrouch();
	bCrouchButtonIsHeld = false;
}

void ABooPawn::StartFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StartPrimaryFire();
	}
}

void ABooPawn::StopFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopPrimaryFire();
	}
}
