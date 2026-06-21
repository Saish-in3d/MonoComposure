// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomDepthStencilMaskCreatorPS.h"
#include "ViewportPassVS.h"

IMPLEMENT_GLOBAL_SHADER(FCustomDepthStencilMaskCreatorPS, "/Plugins/GPUCompositionGraphicsPipeline/CustomDepthStencilMaskCreatorPS.usf", "MainPS", SF_Pixel)

void FCustomDepthStencilMaskCreatorPS::ImplementCustomDepthCustomStencilMaskCreatorShader
(FRDGBuilder& GraphBuilder
		, const FGlobalShaderMap* GlobalShaderMap
		, const FScreenPassRenderTarget& SceneColorRenderTarget
		, const FSceneView& View
		, const FScreenPassTextureViewportParameters& SceneTextureViewportParams
		, const FScreenPassTextureViewport& RegionViewport
		, const FSceneTextureShaderParameters& SceneTextures
		, const TArray<uint32>& StencilIds
		, FScreenPassRenderTarget& OutMergedStencilRenderTarget)
{
	if (StencilIds.Num() == 0)
	{
		return;
	}


	FRDGTextureDesc Desc = SceneColorRenderTarget.Texture->Desc;
	Desc.Format = EPixelFormat::PF_R8_UINT;
	FRDGTexture* MergedStencilTexture = GraphBuilder.CreateTexture(Desc, TEXT("DepthStencil"));
	OutMergedStencilRenderTarget = FScreenPassRenderTarget(
														MergedStencilTexture,
														SceneColorRenderTarget.ViewRect,
														ERenderTargetLoadAction::ENoAction);
	{
		TShaderMapRef<FCustomDepthStencilMaskCreatorPS> StencilMergerPS(GlobalShaderMap);
		TShaderMapRef<FViewportPassVS> StencilMergerVS(GlobalShaderMap);
		FCustomDepthStencilMaskCreatorPS::FParameters* Parameters = GraphBuilder.AllocParameters<FCustomDepthStencilMaskCreatorPS::FParameters>();
		Parameters->SceneTextures = SceneTextures;
		Parameters->RenderTargets[0] = OutMergedStencilRenderTarget.GetRenderTargetBinding();
		Parameters->PostProcessOutput = SceneTextureViewportParams;
		Parameters->View = View.ViewUniformBuffer;

		Parameters->StencilIds = GraphBuilder.CreateSRV(
									FRDGBufferSRVDesc(
									CreateStructuredBuffer(
															GraphBuilder,
															TEXT("DepthStencilBuffer"),
															sizeof(uint32),
															StencilIds.Num(),
															&StencilIds[0],
															sizeof(uint32) * StencilIds.Num())));
		Parameters->StencilIdCount = StencilIds.Num();

		AddDrawScreenPass(
			GraphBuilder,
			RDG_EVENT_NAME("ImplementCustomDepthCustomStencilMaskCreatorShader"),
			View,
			FScreenPassTextureViewport(SceneColorRenderTarget),
			FScreenPassTextureViewport(SceneColorRenderTarget),
			StencilMergerVS,
			StencilMergerPS,
			Parameters
		);
	}
	
	
}