#include "BoomrSkyLight.h"
#include "Components/SkyLightComponent.h"
#include "Engine.h"
#include "Engine/SkyLight.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/TextureCube.h"

ABoomrSkyLight::ABoomrSkyLight()
{
	PrimaryActorTick.bCanEverTick = false;

	SkyLightComponent = CreateDefaultSubobject<USkyLightComponent>(TEXT("SkyLightComponent"));
	RootComponent = SkyLightComponent;

	// Set to use a specified cubemap
	SkyLightComponent->SourceType = SLS_SpecifiedCubemap;
	
	static ConstructorHelpers::FObjectFinder<UTextureCube> WhiteCubemapObj(TEXT("TextureCube'/Game/Maps/MapAssets/WhiteCubemap.WhiteCubemap'"));
	if (WhiteCubemapObj.Succeeded())
	{
		SkyLightComponent->Cubemap = WhiteCubemapObj.Object;
	}

	SkyLightComponent->LightColor = FLinearColor::White.ToFColor(true);
	SkyLightComponent->Intensity = 1.0f;

	// Enable lower hemisphere and set to very dark grey
	SkyLightComponent->bLowerHemisphereIsBlack = true;
	SkyLightComponent->LowerHemisphereColor = FLinearColor(0.02f, 0.02f, 0.02f, 1.0f);

	// Set mobility to Movable since we disabled static lighting
	SkyLightComponent->SetMobility(EComponentMobility::Movable);
}
