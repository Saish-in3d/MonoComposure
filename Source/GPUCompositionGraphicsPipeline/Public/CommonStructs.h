#pragma once
#include "CoreMinimal.h"
#include "ShaderParameterStruct.h"
#include "ScreenPass.h"
#include "SceneTextureParameters.h"
#include "CommonStructs.generated.h"

UENUM(BlueprintType)
enum class ERenderTargetPixelFormat : uint8
{
	RTF_R8          UMETA(DisplayName = "Grayscale (R8)"),
	RTF_RGBA8       UMETA(DisplayName = "Color (RGBA8 - Default)"),
	RTF_RGBA16F     UMETA(DisplayName = "Half Float (RGBA16F)"),
	RTF_FloatRGBA   UMETA(DisplayName = "Full Float HDR (FloatRGBA)"),
	RTF_DepthStencil UMETA(DisplayName = "Depth Stencil")
};

UENUM(BlueprintType)
enum class EVideoResolution : uint8
{
	HD_720p     UMETA(DisplayName = "720p HD (1280x720)"),
	FullHD_1080p UMETA(DisplayName = "1080p Full HD (1920x1080)"),
	QHD_1440p   UMETA(DisplayName = "1440p QHD (2560x1440)"),
	UHD_4K      UMETA(DisplayName = "4K UHD (3840x2160)"),
	UHD_8K      UMETA(DisplayName = "8K UHD (7680x4320)"),
	CUSTOM		UMETA(DisplayName = "Custom")
	
};

USTRUCT(BlueprintType)
struct FCompositorOutputSpecs
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "GPU Composition")
	EVideoResolution OutputResolution;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "GPU Composition")
	FIntVector2 CustomOutputResolutionSize = FIntVector2(500,500);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "GPU Composition")
	float TargetFPS = 50;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "GPU Composition")
	ERenderTargetPixelFormat PixelFormat;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "GPU Composition")
	UTexture2D* SampleTexture;
};

static EPixelFormat GetPixelFormatFromRenderTargetFormat(ERenderTargetPixelFormat PixelFormat)
{
	switch (PixelFormat)
	{
	case ERenderTargetPixelFormat::RTF_R8:           return PF_G8;
	case ERenderTargetPixelFormat::RTF_RGBA8:        return PF_B8G8R8A8;   
	case ERenderTargetPixelFormat::RTF_RGBA16F:      return PF_FloatRGBA;  
	case ERenderTargetPixelFormat::RTF_FloatRGBA:    return PF_FloatRGBA;  
	case ERenderTargetPixelFormat::RTF_DepthStencil: return PF_DepthStencil;
	default:                                         return PF_B8G8R8A8;
	}
}

static FIntVector2 GetOutputResolutionFormatFromRenderTargetFormat(EVideoResolution ResolutionFormat, FIntVector2 CustomResolution)
{
	switch (ResolutionFormat)
	{
	case EVideoResolution::HD_720p: return FIntVector2(1280, 720.0);
	case EVideoResolution::FullHD_1080p: return FIntVector2(1920, 1080);
	case EVideoResolution::QHD_1440p: return FIntVector2(2560, 1440);
	case EVideoResolution::UHD_4K: return FIntVector2(3840, 2160);
	case EVideoResolution::UHD_8K: return FIntVector2(7680, 4320);
	case EVideoResolution::CUSTOM: return FIntVector2(CustomResolution.X, CustomResolution.Y);
	default: return FIntVector2(1920, 1080);
	}
}


BEGIN_SHADER_PARAMETER_STRUCT(FCompositorStruct,)
	SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)
	SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, ViewportSceneParameters)
	SHADER_PARAMETER_RDG_TEXTURE(Texture2D, ViewportSceneColor)
	SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<uint>, MaskInputTexture)
	SHADER_PARAMETER_RDG_TEXTURE(Texture2D, MediaTexture)
	SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, CompOutput)
	SHADER_PARAMETER(int,TransparentBase)
	SHADER_PARAMETER(float,StageOneValue)
	SHADER_PARAMETER(float,StageTwoValue)
	END_SHADER_PARAMETER_STRUCT()


BEGIN_SHADER_PARAMETER_STRUCT(FDepthMaskStruct,)
	SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)
	SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<uint>, StencilIds)
	SHADER_PARAMETER(uint32, StencilIdCount)
	SHADER_PARAMETER_STRUCT_INCLUDE(FSceneTextureShaderParameters, SceneTextures)
	SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, PostProcessOutput)
	RENDER_TARGET_BINDING_SLOTS()
END_SHADER_PARAMETER_STRUCT()