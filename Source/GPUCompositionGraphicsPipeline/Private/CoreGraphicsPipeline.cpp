// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreGraphicsPipeline.h"
#include "CompositorCS.h"
#include "CustomDepthStencilMaskCreatorPS.h"
#include "CompGraphicsSubsystem.h"


#include "CommonRenderResources.h"
#include "Containers/DynamicRHIResourceArray.h"
#include "DynamicResolutionState.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "FXRenderingUtils.h"
#include "MediaTexture.h"
#include "PostProcess/PostProcessInputs.h"
#include "RHI.h"
#include "SceneRenderTargetParameters.h"
#include "SceneView.h"
#include "ScreenPass.h"
#include "Engine/TextureRenderTarget2D.h"

#include "RenderGraphUtils.h"

//todo comment all
FCoreSVE::FCoreSVE(const FAutoRegister& AutoRegister) : FSceneViewExtensionBase(AutoRegister)
{
	UE_LOG(LogTemp, Log, TEXT("registered"));
	
}

void FCoreSVE::PrePostProcessPass_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View,
	const FPostProcessingInputs& Inputs)
{
	FSceneViewExtensionBase::PrePostProcessPass_RenderThread(GraphBuilder, View, Inputs);
}

void FCoreSVE::SubscribeToPostProcessingPass(EPostProcessingPass PassId, const FSceneView& View,
	FAfterPassCallbackDelegateArray& InOutPassCallbacks, bool bIsPassEnabled)
{
	UCompGraphicsSubsystem* CompGraphicsSubsystem = GEngine->GetEngineSubsystem<UCompGraphicsSubsystem>();
	if (!CompGraphicsSubsystem){return; }
	
	if (!CompGraphicsSubsystem->GetShouldCompositorRun()){return; }
	if (PassId == EPostProcessingPass::FXAA)
	{
		InOutPassCallbacks.Add(FAfterPassCallbackDelegate::CreateRaw(this,
			&FCoreSVE::GraphicsCompositing));
	}
}

FScreenPassTexture FCoreSVE::GraphicsCompositing(FRDGBuilder& GraphBuilder, const FSceneView& View,
	const FPostProcessMaterialInputs& Inputs)
{
	
	FGlobalShaderMap* GlobalShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);
	
	const FSceneViewFamily& ViewFamily = *View.Family;

	const FIntRect PrimaryViewRect = UE::FXRenderingUtils::GetRawViewRectUnsafe(View);
	
	const FScreenPassTexture& SceneColor = FScreenPassTexture::CopyFromSlice(GraphBuilder,
		Inputs.GetInput(EPostProcessMaterialInput::SceneColor));
	
	if (!SceneColor.IsValid())
	{
		return FScreenPassTexture();
	}
	FScreenPassRenderTarget SceneColorRenderTarget(SceneColor, ERenderTargetLoadAction::ELoad);
	
	const FScreenPassTextureViewport SceneColorViewport(SceneColor);
	const FScreenPassTextureViewportParameters SceneTextureViewportParams = GetScreenPassTextureViewportParameters(
		SceneColorViewport);
		

	const FScreenPassTextureViewport RegionViewport(SceneColorRenderTarget.Texture);
	
	FSceneTextureShaderParameters SceneTextures = CreateSceneTextureShaderParameters(GraphBuilder, View,
		ESceneTextureSetupMode::CustomDepth);
	
	TArray<uint32> StencilIds ;
	StencilIds.Add(20);
	
	UCompGraphicsSubsystem* CompGraphicsSubsystem = GEngine->GetEngineSubsystem<UCompGraphicsSubsystem>();
	UMediaTexture* MT =  CompGraphicsSubsystem->GetImputMediaTextureInternal();
	if (!MT){return SceneColor;}
	OutputRenderTarget = CompGraphicsSubsystem->GetCompositionOutputRenderTargetInternal();
	
	if (StencilIds.Num() == 0 && !OutputRenderTarget )
	{
		return SceneColor; 
	}
	

	FScreenPassRenderTarget MaskRenderTarget ;
	
	FCustomDepthStencilMaskCreatorPS::ImplementCustomDepthCustomStencilMaskCreatorShader(GraphBuilder,
		GlobalShaderMap,SceneColorRenderTarget,View,SceneTextureViewportParams,RegionViewport,SceneTextures,StencilIds,
		MaskRenderTarget);
	
	
	
	
	return FCompositorCS::ImplementCompShader(GraphBuilder, View, Inputs,OutputRenderTarget, MaskRenderTarget, MT,
	                                          CompGraphicsSubsystem->GetCGOverlayonBackgroundFootageValue(),
	                                          CompGraphicsSubsystem->GetVirtualWorldOverlayValue(),
	                                          CompGraphicsSubsystem->GetShallCompOutputOnViewport(),
	                                          CompGraphicsSubsystem->GetShallUseTransparentMediaTexture());
	
	
	
}