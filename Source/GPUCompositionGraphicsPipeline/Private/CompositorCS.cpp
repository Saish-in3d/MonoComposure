// Fill out your copyright notice in the Description page of Project Settings.


#include "CompositorCS.h"

#include "SceneRenderTargetParameters.h"
//#include "SceneView.h"
#include "Engine/TextureRenderTarget2D.h"
#include "MediaTexture.h"

IMPLEMENT_GLOBAL_SHADER(FCompositorCS, "/Plugins/GPUCompositionGraphicsPipeline/CompositorCS.usf", "CompositorCS", SF_Compute);


FScreenPassTexture FCompositorCS::ImplementCompShader(FRDGBuilder& GraphBuilder, const FSceneView& SceneView,
                                                      const FPostProcessMaterialInputs& Inputs, UTextureRenderTarget2D* CompOutputRenderTarget,
                                                      FScreenPassRenderTarget DepthMask, UMediaTexture* inMediaTexture, float inCompStageOneValue, float inCompStageTwoValue, bool
                                                      bShallCompOutputOnViewport, bool bAlphaOutput)
{
	
	
	//FSceneTextureShaderParameters SceneTextures = CreateSceneTextureShaderParameters(GraphBuilder, SceneView, ESceneTextureSetupMode::All);
	
	const FSceneViewFamily& ViewFamily = *SceneView.Family;
	FScreenPassRenderTarget Output = Inputs.OverrideOutput;
	const FScreenPassTexture& SceneColor = FScreenPassTexture::CopyFromSlice(GraphBuilder, Inputs.GetInput(EPostProcessMaterialInput::SceneColor));
	
	if (!Output.IsValid())
	{
		Output = FScreenPassRenderTarget::CreateFromInput(
			GraphBuilder, SceneColor,
			ERenderTargetLoadAction::ENoAction,
			TEXT("CompositorViewportOutput"));
	}
	
	RDG_EVENT_SCOPE(GraphBuilder, "GPUCoreCompositionPipeline");
	{
		FGlobalShaderMap* GlobalShaderMap = GetGlobalShaderMap(ViewFamily.GetFeatureLevel());
	
		const FScreenPassTextureViewport SceneColorViewport(SceneColor);
		
		FRDGTextureDesc OutputDesc;
		{
			OutputDesc = SceneColor.Texture->Desc;

			//OutputDesc.Reset();
			OutputDesc.Flags |= TexCreate_UAV;
			OutputDesc.Flags &= ~(TexCreate_RenderTargetable | TexCreate_FastVRAM);
			OutputDesc.Extent = SceneColor.ViewRect.Size();

			FLinearColor ClearColor(1., 0., 0., 0.);
			OutputDesc.ClearValue = FClearValueBinding(ClearColor);
		}
		
		FRDGTextureRef MediaTextureRDGTextureRef;
		
		//media texture setup start
		if (!inMediaTexture){return SceneColor;}		
		FTextureResource* MediaTextureResource =  inMediaTexture->GetResource();

		if (!MediaTextureResource){return SceneColor;}
		FRHITexture* MediaTextureRHITexture = MediaTextureResource->GetTexture2DRHI();

		if (!MediaTextureRHITexture){return SceneColor;}
		MediaTextureRDGTextureRef = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(MediaTextureRHITexture,TEXT("MediaTexture")));

		if (!MediaTextureRDGTextureRef){return SceneColor;}
		//media texture setup end
		
		
		
		FRDGTextureRef OutputTexture = GraphBuilder.CreateTexture(OutputDesc, TEXT("Custom Effect Output Texture"));

		// Setting the shader parameters
		FCompositorCS::FParameters* CompComputeShaderParameters = GraphBuilder.AllocParameters<FCompositorCS::FParameters>();
		
		CompComputeShaderParameters->ViewportSceneColor = SceneColor.Texture;
		
		FRDGTextureSRVRef MaskSRV = GraphBuilder.CreateSRV(FRDGTextureSRVDesc::Create(DepthMask.Texture));
		CompComputeShaderParameters->MaskInputTexture = MaskSRV;

		//In case we are not using media texture
		CompComputeShaderParameters->TransparentBase = bAlphaOutput;
		
		//Seting compositing Stage 1 and two blend values
		CompComputeShaderParameters->StageOneValue = inCompStageOneValue;
		CompComputeShaderParameters->StageTwoValue = inCompStageTwoValue;
		
		CompComputeShaderParameters->ViewportSceneParameters = GetScreenPassTextureViewportParameters(SceneColorViewport);

		FIntPoint PassViewSize = SceneColor.ViewRect.Size();
		
		CompComputeShaderParameters->MediaTexture = MediaTextureRDGTextureRef;
		
		CompComputeShaderParameters->CompOutput = GraphBuilder.CreateUAV(FRDGTextureUAVDesc(OutputTexture));
		
		CompComputeShaderParameters->View = SceneView.ViewUniformBuffer;
		
		FIntVector CompComputeShaderGroupCount = FComputeShaderUtils::GetGroupCount(PassViewSize, FComputeShaderUtils::kGolden2DGroupSize);

		TShaderMapRef<FCompositorCS> CompComputeShader(GlobalShaderMap);
		
		FComputeShaderUtils::AddPass(
			GraphBuilder,
			RDG_EVENT_NAME("CompositorPass",
			PassViewSize.X,
			PassViewSize.Y),
			CompComputeShader,
			CompComputeShaderParameters,
			CompComputeShaderGroupCount);
		
		if (CompOutputRenderTarget == nullptr){return SceneColor;}
		
		FTextureRenderTargetResource* RTResource =  CompOutputRenderTarget->GetRenderTargetResource();
		if (RTResource == nullptr){return SceneColor;}
		
		FRHITexture* OutputRTRHITexture = RTResource->GetRenderTargetTexture();
		if (OutputRTRHITexture == nullptr){return SceneColor;}
		
		FRDGTextureRef OutputRTRDGRef = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(OutputRTRHITexture,TEXT("CompOutputRenderTarget")));
		
		FRHICopyTextureInfo CopyInfo;
		
		FIntPoint Src = OutputTexture->Desc.Extent;
		FIntPoint Dst = OutputRTRDGRef->Desc.Extent;
		
		CopyInfo.Size = CopyInfo.Size = FIntVector(FMath::Min(Src.X, Dst.X),FMath::Min(Src.Y, Dst.Y),1);
		
		AddCopyTexturePass(GraphBuilder, OutputTexture, OutputRTRDGRef,CopyInfo);
		//AddCopyTexturePass(GraphBuilder, OutputTexture, Output.Texture);
		AddDrawTexturePass(GraphBuilder, SceneView, FScreenPassTexture(OutputTexture), Output);
		if (bShallCompOutputOnViewport)
		{
			/*return FScreenPassTexture(OutputTexture);*/
			return /*MoveTemp(*/FScreenPassTexture(Output)/*)*/;
		}
	}
	return SceneColor;
	
	
}
