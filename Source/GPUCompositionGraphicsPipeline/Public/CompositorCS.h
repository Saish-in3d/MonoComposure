// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RenderGraphUtils.h"
#include "PostProcess/PostProcessMaterial.h"
#include "DataDrivenShaderPlatformInfo.h"
#include "CommonStructs.h"
#include "MediaTexture.h"

/**
 * 
 */
class GPUCOMPOSITIONGRAPHICSPIPELINE_API FCompositorCS: public FGlobalShader
{
public:
	
	DECLARE_GLOBAL_SHADER(FCompositorCS)
	
	using FParameters = FCompositorStruct;
	SHADER_USE_PARAMETER_STRUCT(FCompositorCS, FGlobalShader)
	
	
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
	
	
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		OutEnvironment.SetDefine(TEXT("CompTHREADS_X"), 8);
		OutEnvironment.SetDefine(TEXT("CompTHREADS_Y"), 8);
		OutEnvironment.SetDefine(TEXT("CompTHREADS_Z"), 1);
	}
	
	static FScreenPassTexture ImplementCompShader(FRDGBuilder& GraphBuilder,
													const FSceneView& SceneView,
													const FPostProcessMaterialInputs& Inputs,
													UTextureRenderTarget2D* CompOutputRenderTarget,
													FScreenPassRenderTarget DepthMask,
													UMediaTexture* inMediaTexture,
													float inCompStageOneValue,
													float inCompStageTwoValue,
													bool bShallCompOutputOnViewport,
													bool bAlphaOutput
													);
};
