#include "BoomrHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"

void ABoomrHUD::BeginPlay()
{
	Super::BeginPlay();
}

void ABoomrHUD::DrawHUD()
{
	Super::DrawHUD();

	DrawFPS();
	DrawCrosshair();
}

void ABoomrHUD::DrawFPS()
{
	if (Canvas)
	{
		float DeltaTime = FApp::GetDeltaTime();
		if (DeltaTime > 0.0f)
		{
			int32 FPS = FMath::RoundToInt(1.0f / DeltaTime);
			FString FPSText = FString::Printf(TEXT("FPS: %d"), FPS);

			float TextScale = 1.5f;
			float TextWidth, TextHeight;
			GetTextSize(FPSText, TextWidth, TextHeight, nullptr, TextScale);

			// Position in top right with a small margin
			float Padding = 5.0f;
			float X = Canvas->SizeX - TextWidth - 20.0f;
			float Y = 20.0f;

			// Draw black background
			DrawRect(FLinearColor::Black, X - Padding, Y - Padding, TextWidth + (Padding * 2.0f), TextHeight + (Padding * 2.0f));

			DrawText(FPSText, FLinearColor::White, X, Y, nullptr, TextScale);
		}
	}
}

void ABoomrHUD::DrawCrosshair()
{
	if (Canvas)
	{
		float CenterX = Canvas->SizeX / 2.0f;
		float CenterY = Canvas->SizeY / 2.0f;
		float LineLength = 10.0f; // 20px total (10px each side)

		// Horizontal line
		// Black outline
		DrawLine(CenterX - LineLength - 1.0f, CenterY, CenterX + LineLength + 1.0f, CenterY, FLinearColor::Black, 4.0f);
		// White center
		DrawLine(CenterX - LineLength, CenterY, CenterX + LineLength, CenterY, FLinearColor::White, 2.0f);
		
		// Vertical line
		// Black outline
		DrawLine(CenterX, CenterY - LineLength - 1.0f, CenterX, CenterY + LineLength + 1.0f, FLinearColor::Black, 4.0f);
		// White center
		DrawLine(CenterX, CenterY - LineLength, CenterX, CenterY + LineLength, FLinearColor::White, 2.0f);
	}
}
