// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShaderParameterStruct.h"
#include "CommonStructs.h"
#include "PostProcess/PostProcessMaterial.h"
#include "DataDrivenShaderPlatformInfo.h"

/**
 * 
 */

class FCustomDepthStencilMaskCreatorPS: public  FGlobalShader
{
	DECLARE_GLOBAL_SHADER(FCustomDepthStencilMaskCreatorPS);
	
	using FParameters = FDepthMaskStruct;
	SHADER_USE_PARAMETER_STRUCT(FCustomDepthStencilMaskCreatorPS, FGlobalShader);
	
	

	
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return !IsMobilePlatform(Parameters.Platform);
	}
	
	static void ModifyCompilationEnvironment(
	   const FGlobalShaderPermutationParameters& Parameters,
	   FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	}

public:
	static void ImplementCustomDepthCustomStencilMaskCreatorShader
	(FRDGBuilder& GraphBuilder
		, const FGlobalShaderMap* GlobalShaderMap
		, const FScreenPassRenderTarget& SceneColorRenderTarget
		, const FSceneView& View
		, const FScreenPassTextureViewportParameters& SceneTextureViewportParams
		, const FScreenPassTextureViewport& RegionViewport
		, const FSceneTextureShaderParameters& SceneTextures
		, const TArray<uint32>& StencilIds
		, FScreenPassRenderTarget& OutMergedStencilRenderTarget);
	 
};
