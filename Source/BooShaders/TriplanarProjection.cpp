#include "TriplanarProjection.h"
#include "MaterialCompiler.h"

UTriplanarProjection::UTriplanarProjection(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer) {
	Texture = nullptr;
	ScaleDefault = 0.005f;
	BlendSharpnessDefault = 4.0f;
}
#if WITH_EDITOR
const TArray<FExpressionInput *> UTriplanarProjection::GetInputs() {
	TArray<FExpressionInput *> Out;
	Out.Add(&TextureObject);
	Out.Add(&Scale);
	Out.Add(&BlendSharpness);
	return Out;
}
FExpressionInput *UTriplanarProjection::GetInput(int32 InputIndex) {
	switch (InputIndex) {
	case 0:
		return &TextureObject;
	case 1:
		return &Scale;
	case 2:
		return &BlendSharpness;
	default:
		return nullptr;
	}
}
FName UTriplanarProjection::GetInputName(int32 InputIndex) const {
	switch (InputIndex) {
	case 0:
		return TEXT("Texture");
	case 1:
		return TEXT("Scale");
	case 2:
		return TEXT("BlendSharpness");
	default:
		return NAME_None;
	}
}
uint32 UTriplanarProjection::GetInputType(int32 InputIndex) {
	switch (InputIndex) {
	case 0:
		return MCT_Texture;
	case 1:
		return MCT_Float1;
	case 2:
		return MCT_Float1;
	default:
		return MCT_Unknown;
	}
}
int32 UTriplanarProjection::Compile(FMaterialCompiler *Compiler, int32 OutputIndex) {
	int32 TextureCode = INDEX_NONE;
	if (TextureObject.IsConnected()) {
		TextureCode = TextureObject.Compile(Compiler);
	} else if (Texture) {
		TextureCode = Compiler->Texture(Texture, SAMPLERTYPE_Color);
	} else {
		return Compiler->Error(TEXT("Triplanar: Texture is not specified."));
	}
	const int32 WP = Compiler->WorldPosition(WPT_Default);

	const int32 ScaleCode = Scale.IsConnected() ? Scale.Compile(Compiler) : Compiler->Constant(ScaleDefault);

	int32 UVx = Compiler->ComponentMask(WP, false, true, true, false);
	UVx = Compiler->Mul(UVx, ScaleCode);

	int32 UVy = Compiler->ComponentMask(WP, true, false, true, false);
	UVy = Compiler->Mul(UVy, ScaleCode);

	int32 UVz = Compiler->ComponentMask(WP, true, true, false, false);
	UVz = Compiler->Mul(UVz, ScaleCode);

	int32 N = Compiler->VertexNormal();
	int32 NAbs = Compiler->Abs(N);

	const int32 Sharp = BlendSharpness.IsConnected() ? BlendSharpness.Compile(Compiler)
	                                                 : Compiler->Constant(BlendSharpnessDefault);

	int32 Weights = Compiler->Power(NAbs, Sharp);
	int32 Ones = Compiler->Constant3(1.0f, 1.0f, 1.0f);
	int32 Sum = Compiler->Dot(Weights, Ones);
	int32 WeightsN = Compiler->Div(Weights, Sum);

	int32 Wx = Compiler->ComponentMask(WeightsN, true, false, false, false);
	int32 Wy = Compiler->ComponentMask(WeightsN, false, true, false, false);
	int32 Wz = Compiler->ComponentMask(WeightsN, false, false, true, false);

	const EMaterialSamplerType SamplerType = SAMPLERTYPE_Color;
	const ESamplerSourceMode SamplerSource = SSM_FromTextureAsset;

	int32 Sx = Compiler->TextureSample(TextureCode, UVx, SamplerType, 
		INDEX_NONE, INDEX_NONE, TMVM_None, SamplerSource);
	int32 Sy = Compiler->TextureSample(TextureCode, UVy, SamplerType, 
		INDEX_NONE, INDEX_NONE, TMVM_None, SamplerSource);
	int32 Sz = Compiler->TextureSample(TextureCode, UVz, SamplerType, 
		INDEX_NONE, INDEX_NONE, TMVM_None, SamplerSource);

	int32 OutX = Compiler->Mul(Sx, Wx);
	int32 OutY = Compiler->Mul(Sy, Wy);
	int32 OutZ = Compiler->Mul(Sz, Wz);

	int32 OutXY = Compiler->Add(OutX, OutY);
	int32 OutXYZ = Compiler->Add(OutXY, OutZ);

	return OutXYZ;
}
#endif
