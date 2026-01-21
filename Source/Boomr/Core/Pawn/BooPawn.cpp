#include "BooPawn.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "Grenade.h"
#include "WeaponStatic.h"


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
	Camera->bUsePawnControlRotation = true;

    bJumpButtonIsHeld = false;
    bCrouchButtonIsHeld = false;
    CharacterMovementProps = FCharacterMovementProperties();
    ApplyCharacterMovementProperties(CharacterMovementProps);

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
	PlayerInputComponent->BindAction("Shoot", IE_Pressed, this, &ABooPawn::Shoot);
	//throw grenade
        PlayerInputComponent->BindAction("ThrowGrenade", IE_Pressed, this, &ABooPawn::ThrowGrenade);

	// turn and look
	PlayerInputComponent->BindAxis("Turn", this, &ABooPawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &ABooPawn::AddControllerPitchInput);

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

void ABooPawn::Shoot()
{
	UWeaponStatic::FireHitscan(
		GetWorld(),
		this,
		GetController(),	
		Camera->GetComponentLocation(),
		Camera->GetForwardVector()
	);
}
void ABooPawn::ThrowGrenade()
{
	if (GrenadeClass)
	{
		FVector SpawnLocation = Camera->GetComponentLocation();
		FRotator SpawnRotation = Camera->GetComponentRotation();

		AGrenade* Grenade = GetWorld()->SpawnActor<AGrenade>(GrenadeClass, SpawnLocation, SpawnRotation);
		if (Grenade)
		{
			Grenade->SetOwner(this);

		}
	}
}
