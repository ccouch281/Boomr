#pragma once

// Inherits from APawn
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/DamageType.h"
#include "BooPawn.generated.h"

class USkeletalMeshComponent;
class UCameraComponent;
class UInputManager;

class UMaterialInterface;
class USoundBase;
class AGrenade;

USTRUCT(BlueprintType)
struct FCharacterMovementProperties
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gametype)
	float WalkingMaxSpeed = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gametype)
	float CrouchWalkingMaxSpeed = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gametype)
	float WalkingAcceleration = 2000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gametype)
	float WalkingDeceleration = 2200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gametype)
	float BrakingFrictionFactor = 1.7f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gametype)
	float WalkableFloorAngle = 45.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gametype)
	float GroundFriction = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gametype)
	bool bCanWalkOffLedgesWhenCrouching = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gametype)
	bool bMaintainHorizontalGroundVelocity = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gametype)
	float JumpZVelocity = 620.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gametype)
	float BrakingDecelerationFalling = 15.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gametype)
	float AirControl = 0.09f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gametype)
	float AirControlBoostMultiplier = 1.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gametype)
	float AirControlBoostVelocityThreshold = 400.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gametype)
	float FallingLateralFriction = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gametype)
	float Gravity = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gametype)
	bool bCanCrouch = true;
};

UCLASS()
class BOOMR_API ABooPawn : public ACharacter
{
    GENERATED_BODY()

public:
    ABooPawn();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	UCameraComponent* Camera;

	void ApplyCharacterMovementProperties(const FCharacterMovementProperties& MovementProperties);
	FCharacterMovementProperties CharacterMovementProps;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void MoveBack(float Value);
	void MoveLeft(float Value);
	
	void AddYawInput(float Value);
	void AddPitchInput(float Value);

	void MyStartJumping();
	void MyStopJumping();
	bool bJumpButtonIsHeld;
	void MyStartCrouching();
	void MyStopCrouching();
	bool bCrouchButtonIsHeld;

	void Shoot();

    void ThrowGrenade();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grenade")
    TSubclassOf<AGrenade> GrenadeClass;

};