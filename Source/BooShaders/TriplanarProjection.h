#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture.h"
#include "MaterialExpressionIO.h"
#include "Materials/MaterialExpression.h"
#include "UObject/ObjectMacros.h"
#include "TriplanarProjection.generated.h"

UCLASS(collapsecategories, hidecategories = Object)
class BOOSHADERS_API UTriplanarProjection : public UMaterialExpression {
	GENERATED_UCLASS_BODY()

	/** Optional texture object input which overrides Texture if specified. */
	UPROPERTY(meta = (RequiredInput = "false", ToolTip = "Defaults to 'Texture' if not specified"))
	FExpressionInput TextureObject;

	/** World-space tiling scale for projection (tiles per world unit). Defaults to 1/100 so the
	 * texture spans 100 units by default. */
	UPROPERTY(meta = (RequiredInput = "false", ToolTip = "Tiles per world unit. Defaults to 1/100 (one tile per 100 units) if not specified"))
	FExpressionInput Scale;

	/** Blend sharpness for axis weights. Higher = sharper transitions. */
	UPROPERTY(meta = (RequiredInput = "false", ToolTip = "Defaults to 'BlendSharpnessDefault' if not specified"))
	FExpressionInput BlendSharpness;

	/** Fallback texture when TextureObject is not connected. */
	UPROPERTY(EditAnywhere, Category = Triplanar, meta = (OverridingInputProperty = "TextureObject"))
	UTexture *Texture;

	/** Default tiling scale when Scale is unconnected (tiles per world unit). */
	UPROPERTY(EditAnywhere, Category = Triplanar, meta = (OverridingInputProperty = "Scale"))
	float ScaleDefault;

	/** Default blend sharpness when BlendSharpness is unconnected. */
	UPROPERTY(EditAnywhere, Category = Triplanar, meta = (OverridingInputProperty = "BlendSharpness"))
	float BlendSharpnessDefault;

#if WITH_EDITOR
	//~ Begin UMaterialExpression Interface
	virtual const TArray<FExpressionInput *> GetInputs() override;
	virtual FExpressionInput *GetInput(int32 InputIndex) override;
	virtual FName GetInputName(int32 InputIndex) const override;
	virtual uint32 GetInputType(int32 InputIndex) override;
	virtual int32 Compile(class FMaterialCompiler *Compiler, int32 OutputIndex) override;
	virtual void GetCaption(TArray<FString> &OutCaptions) const override {
		OutCaptions.Add(TEXT("Triplanar Projection"));
	}
#endif
	//~ End UMaterialExpression Interface
};

