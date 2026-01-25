#include "BooPlayerController.h"
#include "Framework/Application/SlateApplication.h"
#include "Engine/Engine.h"
#include "BoomrRawInput.h"
#include "RawInputMessageHandler.h"
#include "BooPawn.h"
#include "Camera/CameraComponent.h"
#include "HAL/IConsoleManager.h"

ABooPlayerController::ABooPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	// Initial calculation
	HalfSensitivity = (BaseSensitivity * UserSensitivity) * 0.5;
}

void ABooPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Force sensitivity recalculation in case it was modified in Blueprint
	SetMouseSensitivity(UserSensitivity);

	// Sync our absolute buffer to the initial spawn rotation
	SyncOrientationToActor();
}

void ABooPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FBoomrRawInputModule& RawInputModule = FModuleManager::GetModuleChecked<FBoomrRawInputModule>("BoomrRawInput");
	TSharedPtr<FRawInputMessageHandler> Handler = RawInputModule.GetRawInputHandler();

	if (Handler.IsValid())
	{
		// 1. Get current absolute counts
		FIntPoint64 TotalMovement = Handler->GetTotalAccumulation();

		// 2. Pitch Clamping (Enforce absolute physical limits)
		// We calculate the actual angle to check against the limit
		const double Sensitivity = HalfSensitivity * 2.0;
		double CurrentPitch = (double)TotalMovement.Y * Sensitivity;
		const double PitchLimitRad = FMath::DegreesToRadians(89.0);

		if (FMath::Abs(CurrentPitch) > PitchLimitRad)
		{
			CurrentPitch = FMath::Clamp(CurrentPitch, -PitchLimitRad, PitchLimitRad);
			// Back-calculate the integer count and sync the hardware buffer
			TotalMovement.Y = (int64)FMath::RoundToDouble(CurrentPitch / Sensitivity);
			Handler->SetAccumulation(TotalMovement.X, TotalMovement.Y);
		}

		if (Handler->ConsumeClick(PendingShootCounts))
		{
			bHasPendingShoot = true;
		}

		if (ABooPawn* MyPawn = Cast<ABooPawn>(GetPawn()))
		{
			// Update internal state
			FQuat FullQuat = GetQuatFromCounts(TotalMovement);
			VirtualRotation.W = (double)FullQuat.W;
			VirtualRotation.X = (double)FullQuat.X;
			VirtualRotation.Y = (double)FullQuat.Y;
			VirtualRotation.Z = (double)FullQuat.Z;

			// Yaw-Only quaternion for the Body [0, 0, sin(Y/2), cos(Y/2)]
			const double HalfYaw = (double)TotalMovement.X * HalfSensitivity;
			FQuat YawOnlyQuat(0.0f, 0.0f, (float)sin(HalfYaw), (float)cos(HalfYaw));

			MyPawn->SetActorRotation(YawOnlyQuat, ETeleportType::None);

			if (MyPawn->Camera)
			{
				MyPawn->Camera->SetWorldRotation(FullQuat);

				// Handle Scroll Zoom (High-Precision physical integration)
				// 120 counts = 1 standard mouse wheel notch
				FIntPoint64 ScrollAccum = Handler->GetScrollAccumulation();
				if (FMath::Abs(ScrollAccum.Y) >= 120)
				{
					float ZoomDelta = (float)(ScrollAccum.Y / 120) * 5.0f;
					MyPawn->Camera->FieldOfView = FMath::Clamp(MyPawn->Camera->FieldOfView - ZoomDelta, 30.0f, 110.0f);
					Handler->ResetScroll();
				}
			}
		}
	}
}

void ABooPlayerController::SetMouseSensitivity(double NewSensitivity)
{
	UserSensitivity = NewSensitivity;
	HalfSensitivity = (BaseSensitivity * UserSensitivity) * 0.5;
}

void ABooPlayerController::SyncOrientationToActor()
{
	if (APawn* MyPawn = GetPawn())
	{
		FRotator ActorRot = MyPawn->GetActorRotation();
		
		// Convert initial rotation to counts
		// Note: Unreal Rotators use degrees; we need radians
		double CurrentYawRad = FMath::DegreesToRadians(ActorRot.Yaw);
		double CurrentPitchRad = FMath::DegreesToRadians(ActorRot.Pitch);

		const double Sensitivity = HalfSensitivity * 2.0;

		int64 NewX = (int64)FMath::RoundToDouble(CurrentYawRad / Sensitivity);
		int64 NewY = (int64)FMath::RoundToDouble(CurrentPitchRad / Sensitivity);

		FBoomrRawInputModule& RawInputModule = FModuleManager::GetModuleChecked<FBoomrRawInputModule>("BoomrRawInput");
		TSharedPtr<FRawInputMessageHandler> Handler = RawInputModule.GetRawInputHandler();

		if (Handler.IsValid())
		{
			Handler->SetAccumulation(NewX, NewY);
		}
	}
}

void ABooPlayerController::GetHighPrecisionFireTransform(FVector& OutLocation, FVector& OutDirection)
{
	if (ABooPawn* MyPawn = Cast<ABooPawn>(GetPawn()))
	{
		OutLocation = MyPawn->Camera->GetComponentLocation();
		
		FIntPoint64 TargetCounts;
		bool bValidCounts = false;

		// 1. Try to fetch fresh hardware data to bypass input lag
		FBoomrRawInputModule& RawInputModule = FModuleManager::GetModuleChecked<FBoomrRawInputModule>("BoomrRawInput");
		TSharedPtr<FRawInputMessageHandler> Handler = RawInputModule.GetRawInputHandler();

		if (Handler.IsValid())
		{
			// If we haven't consumed the click for the frame yet, do it now
			if (!bHasPendingShoot)
			{
				bHasPendingShoot = Handler->ConsumeClick(PendingShootCounts);
			}

			if (bHasPendingShoot)
			{
				TargetCounts = PendingShootCounts;
				bValidCounts = true;
				bHasPendingShoot = false; // Reset for this shot
			}
			else
			{
				// No pending click? Use the absolute latest current hardware counts
				TargetCounts = Handler->GetTotalAccumulation();
				bValidCounts = true;
			}
		}

		if (bValidCounts)
		{
			OutDirection = GetQuatFromCounts(TargetCounts).GetForwardVector();
		}
		else
		{
			// Absolute fallback to 32-bit state if HID is failing
			OutDirection = MyPawn->Camera->GetForwardVector();
		}
	}
}

FQuat ABooPlayerController::GetQuatFromCounts(const FIntPoint64& Counts) const
{
	const double HY = (double)Counts.X * HalfSensitivity;
	const double HP = (double)Counts.Y * HalfSensitivity;

	const double sY = sin(HY);
	const double cY = cos(HY);
	const double sP = sin(HP);
	const double cP = cos(HP);

	// Yaw * Pitch with zero Roll
	// Order: X, Y, Z, W
	return FQuat(
		(float)(-sY * sP),
		(float)(cY * sP),
		(float)(sY * cP),
		(float)(cY * cP)
	);
}

void ABooPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (InputComponent)
	{
		// The Raw Input pipeline bypasses the standard BindAxis 
		// but we still use the InputComponent for standard actions if needed.
	}

	// Register high-precision sensitivity control console command
	IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("Boomr.Sensitivity"),
		TEXT("Sets the mouse sensitivity multiplier for the 64-bit pipeline."),
		FConsoleCommandWithArgsDelegate::CreateLambda([this](const TArray<FString>& Args)
		{
			if (Args.Num() > 0)
			{
				double NewSens = FCString::Atod(*Args[0]);
				SetMouseSensitivity(NewSens);
				
				if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, 
						FString::Printf(TEXT("Sensitivity set to: %f"), UserSensitivity));
				}
			}
		}),
		ECVF_Default
	);
}

