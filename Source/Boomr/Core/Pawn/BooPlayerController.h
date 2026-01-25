#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BoomrRawInput/RawInputMessageHandler.h"

#include "BooPlayerController.generated.h"

struct FQuat64
{
	double W, X, Y, Z;
	FQuat64() : W(1.0), X(0.0), Y(0.0), Z(0.0) {}

	/** Converts our double-precision state to an engine-compatible float quaternion. */
	FORCEINLINE FQuat ToFQuat() const { return FQuat((float)X, (float)Y, (float)Z, (float)W); }
};

UCLASS()
class BOOMR_API ABooPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ABooPlayerController();

	/** Sets the user's preferred sensitivity and pre-calculations for the pipeline. */
	void SetMouseSensitivity(double NewSensitivity);

	/** Synchronizes the internal orientation with the actor's current rotation. 
	 *  Essential when spawning or teleporting.
	 */
	void SyncOrientationToActor();

	/** Retrieves the exact orientation for a shot, using sub-tick captured data if available. */
	void GetHighPrecisionFireTransform(FVector& OutLocation, FVector& OutDirection);

	/** Shared math for converting raw counts to a zero-roll orientation quaternion. */
	FQuat GetQuatFromCounts(const FIntPoint64& Counts) const;

protected:
	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	/** Base sensitivity multiplier (rads per count). 
	 *  0.00005 is a common baseline for 800-1600 DPI.
	 */
	UPROPERTY(EditAnywhere, Category = "Input")
	double BaseSensitivity = 0.00005;

	/** User-facing sensitivity multiplier. */
	UPROPERTY(EditAnywhere, Category = "Input")
	double UserSensitivity = 1.0;

	/** Pre-calculated (TotalSensitivity * 0.5) to eliminate redundant per-frame math. */
	double HalfSensitivity;

	/** The high-precision orientation state in 64-bit space */
	FQuat64 VirtualRotation;

	/** Sub-tick click state */
	FIntPoint64 PendingShootCounts;
	bool bHasPendingShoot = false;
};
