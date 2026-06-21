// Fill out your copyright notice in the Description page of Project Settings.


#include "CompGraphicsSubsystem.h"
#include "CoreGraphicsPipeline.h"
#include "Kismet/KismetSystemLibrary.h"
#include "MediaTexture.h"
#include "Components/LightComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Kismet/GameplayStatics.h" 
#include "Engine/World.h"
#include "Slate/SceneViewport.h"




void UCompGraphicsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	//InitilizeCompositor();
}

void UCompGraphicsSubsystem::InitilizeCompositor()
{
	//Enabling custom depth and setting it to " Custom-Depth Pass with Stencil"
	UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), FString("r.CustomDepth 3"));

	
	CoreSVE = FSceneViewExtensions::NewExtension<FCoreSVE>();
	UE_LOG(LogTemp, Log, TEXT("Subsystem and SVE initialised"));
}

void UCompGraphicsSubsystem::StartCompositor()
{
	bShouldCompositorRun = true;
}

void UCompGraphicsSubsystem::EndCompositor()
{
	bShouldCompositorRun = false;
	SetCompositionResolution(0,0);
}

bool UCompGraphicsSubsystem::GetShouldCompositorRun()
{
	return bShouldCompositorRun;
}

void UCompGraphicsSubsystem::Deinitialize()
{
	DeinitializeCompositor();
}

void UCompGraphicsSubsystem::SetInputMediaTexture(UMediaTexture* inMediaTexture, bool TransparentMediaTexture)
{
	if (inMediaTexture)
	{
		MediaTexture = inMediaTexture;
	}
	bUseTransparentMediaTexture = TransparentMediaTexture;
}

UTextureRenderTarget2D* UCompGraphicsSubsystem::GetCompositionOutputRenderTarget()
{
	if (OutputRenderTarget)
	{
		return OutputRenderTarget;
	}
	else{return nullptr;}
}



void UCompGraphicsSubsystem::DeinitializeCompositor()
{
	if (CoreSVE.IsValid())
	{
		
		CoreSVE->IsActiveThisFrameFunctions.Empty();
	}

	CoreSVE.Reset();
	CoreSVE = nullptr;
	
	OutputRenderTarget = nullptr;
	
	OutputRenderTarget = nullptr;
	
	ShowOnlyActorsArray.Empty();
	//OutputRenderTarget = nullptr;
	
}

void UCompGraphicsSubsystem::SetShowOnlyActors(TArray<AActor*> inActorsArray)
{
	ResetShowOnlyActors();
	ShowOnlyActorsArray = inActorsArray;
	SetVisiblityOfAllActors(false);
	SetVisiblityShowOnlyActors(true);
	ProcessShowOnlyActors();
	
}

void UCompGraphicsSubsystem::ResetShowOnlyActors()
{
	for(AActor* ShowonlyActor : ShowOnlyActorsArray)
	{
		if (ShowonlyActor && !ShowonlyActor->IsPendingKillPending())
		{
		 	TArray<UPrimitiveComponent*> PrimitiveComponents; 
			ShowonlyActor->GetComponents<UPrimitiveComponent*>(PrimitiveComponents,true);
		    for (UPrimitiveComponent* PrimitiveComponent : PrimitiveComponents)
		    {
		    	if (PrimitiveComponent)
		    	{
		    		PrimitiveComponent->SetRenderCustomDepth(false);
		    		PrimitiveComponent->SetCustomDepthStencilValue(0);//Hardcoded for now
		    		PrimitiveComponent->MarkRenderStateDirty();
		    	}
		    }
			
		}
	}
	
	
}

void UCompGraphicsSubsystem::AddShowOnlyActor(AActor* inActor)
{
	if(inActor && !inActor->IsPendingKillPending() && !ShowOnlyActorsArray.Contains(inActor))
	{
		ShowOnlyActorsArray.Add(inActor);
	}
	SetVisiblityShowOnlyActors(true);
	ProcessShowOnlyActors();//todo: Can be optimised to process a single actor
	
	
}

void UCompGraphicsSubsystem::RemoveShowOnlyActor(AActor* inActor)
{
	if(inActor && !inActor->IsPendingKillPending() && ShowOnlyActorsArray.Contains(inActor))
	{
		ShowOnlyActorsArray.Remove(inActor);
	}
	SetVisiblityOfAllActors(false);
	SetVisiblityShowOnlyActors(true);
	ProcessShowOnlyActors();//todo: Can be optimised to process a single actor
	
}

void UCompGraphicsSubsystem::SetCGOverlayonBackgroundFootageValue(float Value)
{
	
	CGOverlayonBackgroundFootageValue = FMath::Clamp(Value, 0.0f, 1.0f);;	
}

void UCompGraphicsSubsystem::SetVirtualWorldOverlayValue(float Value)
{
	
	VirtualWorldOverlayValue = FMath::Clamp(Value, 0.0f, 1.0f);;
	if (VirtualWorldOverlayValue == 0)
	{
		//Hide all actors
		SetVisiblityOfAllActors(false);
		//Show only actors from ShowOnlyActorsArray
		SetVisiblityShowOnlyActors(true);
	}
	else
	{
		//Show all actors
		SetVisiblityOfAllActors(true);
	}
	
}

void UCompGraphicsSubsystem::SetCompositionResolution(float X, float Y)
{
	if (!GEngine){return;}
	if (!GEngine->GameViewport){return;}
	FViewport* RawViewport = GEngine->GameViewport->Viewport;
	
	if (!RawViewport){return;}
	FSceneViewport* SceneViewport = static_cast<FSceneViewport*>(RawViewport);
	
	if (!SceneViewport){return;}
	SceneViewport->SetFixedViewportSize(X,Y);
	
}

float UCompGraphicsSubsystem::GetCGOverlayonBackgroundFootageValue()
{
	return CGOverlayonBackgroundFootageValue;
	
}

float UCompGraphicsSubsystem::GetVirtualWorldOverlayValue()
{
	
	return VirtualWorldOverlayValue;
}

bool UCompGraphicsSubsystem::GetShallCompOutputOnViewport()
{
	return bShallCompOutputOnViewport;
}

void  UCompGraphicsSubsystem::ToggleCompOutputOnViewport(bool inShallCompOutputOnViewport)
{
	bShallCompOutputOnViewport = inShallCompOutputOnViewport;
}

UMediaTexture* UCompGraphicsSubsystem::GetImputMediaTextureInternal()
{
	if (MediaTexture)
	{
		return MediaTexture;
	}
	else{return nullptr;}
}

UTextureRenderTarget2D* UCompGraphicsSubsystem::GetCompositionOutputRenderTargetInternal()
{
	if (OutputRenderTarget)
	{
		return OutputRenderTarget;
	}
	else{return nullptr;}
}

bool UCompGraphicsSubsystem::GetShallUseTransparentMediaTexture()
{
	return bUseTransparentMediaTexture;
}

void UCompGraphicsSubsystem::ConfigureCompositorOutput(FCompositorOutputSpecs inCompositorOutputSpecs)
{
	
	//Use OutputRenderTarget  in core pipeline (use getter function)
	//Get viewport resaolution
	//If it's same as config res, pass scenecolor/mask for now else pass dummy textures.
	
	CreateRenderTarget(inCompositorOutputSpecs);
}

UTextureRenderTarget2D* UCompGraphicsSubsystem::CreateRenderTarget(FCompositorOutputSpecs inCompositorOutputSpecs)
{
	UTexture2D* TempSourceTexture;
	TempSourceTexture = UTexture2D::CreateTransient(inCompositorOutputSpecs.CustomOutputResolutionSize.X,
		inCompositorOutputSpecs.CustomOutputResolutionSize.Y,EPixelFormat::PF_R16G16B16A16_UINT,FName("ff"));
	
	switch (inCompositorOutputSpecs.OutputResolution)
	{
	case EVideoResolution::HD_720p: TempSourceTexture = LoadObject<UTexture2D>(nullptr,
	TEXT("/GPUCompositionGraphicsPipeline/Textures/HDDummy.HDDummy")); 
		SetCompositionResolution(1280,720);break ;
	case EVideoResolution::FullHD_1080p: TempSourceTexture = LoadObject<UTexture2D>(nullptr,
	TEXT("/GPUCompositionGraphicsPipeline/Textures/FHDDummy.FHDDummy")); 
		SetCompositionResolution(1920,1080);break ;
	case EVideoResolution::QHD_1440p: TempSourceTexture = LoadObject<UTexture2D>(nullptr,
	TEXT("/GPUCompositionGraphicsPipeline/Textures/2KDummy.2KDummy")); 
		SetCompositionResolution(2560,1440);break ;
	case EVideoResolution::UHD_4K: TempSourceTexture = LoadObject<UTexture2D>(nullptr,
	TEXT("/GPUCompositionGraphicsPipeline/Textures/4KDummy.4KDummy")); 
		SetCompositionResolution(3840,2160);break ;
	case EVideoResolution::UHD_8K: TempSourceTexture = LoadObject<UTexture2D>(nullptr,
	TEXT("/GPUCompositionGraphicsPipeline/Textures/8KDummy.8KDummy")); 
		SetCompositionResolution(7680,4320);break ;
	case EVideoResolution::CUSTOM: SetCompositionResolution(inCompositorOutputSpecs.CustomOutputResolutionSize.X,
		inCompositorOutputSpecs.CustomOutputResolutionSize.Y);break ;
	default: TempSourceTexture = LoadObject<UTexture2D>(nullptr,
TEXT("/GPUCompositionGraphicsPipeline/Textures/4KDummy.4KDummy")); break ;
	}


	//Render target setup start
	UTextureRenderTarget2D* TempOutputRenderTarget = NewObject<UTextureRenderTarget2D>(this);
	
	EPixelFormat TempFormat = GetPixelFormatFromRenderTargetFormat(inCompositorOutputSpecs.PixelFormat);
	FIntVector2 TempResolution = GetOutputResolutionFormatFromRenderTargetFormat(
		inCompositorOutputSpecs.OutputResolution, inCompositorOutputSpecs.CustomOutputResolutionSize);
	
	TempOutputRenderTarget->InitCustomFormat(TempResolution.X, TempResolution.Y, TempFormat,true);
	TempOutputRenderTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA16f;
	//Todo make enum to ETextureRenderTargetFormat converter
	TempOutputRenderTarget->ClearColor = FLinearColor::Blue;
	TempOutputRenderTarget->bGPUSharedFlag = false;
	TempOutputRenderTarget->bAutoGenerateMips = false;
	
	
	TempOutputRenderTarget->UpdateResourceImmediate();
	TempOutputRenderTarget->ClearColor = FLinearColor::MakeRandomColor();
	TempOutputRenderTarget->AddToRoot();
	TempOutputRenderTarget->UpdateResource();
	//Render target setup completed
	
	if (!TempSourceTexture){return TempOutputRenderTarget;}

	CopyTexture2DtoRenderTarget(TempOutputRenderTarget,TempSourceTexture);
	
	OutputRenderTarget = TempOutputRenderTarget;
	return TempOutputRenderTarget;
}

void UCompGraphicsSubsystem::CopyTexture2DtoRenderTarget(UTextureRenderTarget2D* DestinationRenderTarget,
	UTexture2D* SourceTexture)
{
	if (!SourceTexture )
	{
		return;
	}
	if (!DestinationRenderTarget)
	{
		return;
	}
	UWorld* World = GWorld;
	if (!World)
	{
		return;
	}
	
	//Loading material from plugin content folder
	
	UMaterialInterface* TextureImporterMaterial = LoadObject<UMaterialInterface>(nullptr,
		TEXT("/GPUCompositionGraphicsPipeline/Materials/M_TextureImportHelper.M_TextureImportHelper"));
	//Creating material dynamic instance
	if (!TextureImporterMaterial){return;}
	UMaterialInstanceDynamic* DynamicMaterialInstance = UKismetMaterialLibrary::CreateDynamicMaterialInstance(World,
		TextureImporterMaterial);
	if (!DynamicMaterialInstance){return;}
	//Assigning the texture to the dynamic material parameter
	DynamicMaterialInstance->SetTextureParameterValue(FName("SourceTexture"), SourceTexture);
	//Drawing material to render target
	UKismetRenderingLibrary::DrawMaterialToRenderTarget(World,DestinationRenderTarget,
		DynamicMaterialInstance);
	
}

void UCompGraphicsSubsystem::ProcessShowOnlyActors()
{
	for(AActor* ShowonlyActor : ShowOnlyActorsArray)
	{
		if (ShowonlyActor && !ShowonlyActor->IsPendingKillPending())
		{
			TArray<UPrimitiveComponent*> PrimitiveComponents; 
			ShowonlyActor->GetComponents<UPrimitiveComponent*>(PrimitiveComponents,true);
			for (UPrimitiveComponent* PrimitiveComponent : PrimitiveComponents)
			{
				if (PrimitiveComponent)
				{
					PrimitiveComponent->SetRenderCustomDepth(true);
					PrimitiveComponent->SetCustomDepthStencilValue(20);//Hardcoded for now
					PrimitiveComponent->MarkRenderStateDirty();
				}
			}
			
		}
	}
	
	
}

void UCompGraphicsSubsystem::SetVisiblityOfAllActors(bool bVisiblity)
{
	UWorld* World = GWorld;
	if (!World){return;}
	
	TArray<AActor*> AllActors;
	UGameplayStatics::GetAllActorsOfClass(World,AActor::StaticClass(),AllActors);
	for (AActor* Actor : AllActors)
	{
		if (Actor && !Actor->IsPendingKillPending() && !(Actor->FindComponentByClass<ULightComponent>()))
		{
			
			Actor->SetActorHiddenInGame(!bVisiblity);
		}
	}
	
	
	
	
}
 
void UCompGraphicsSubsystem::SetVisiblityShowOnlyActors(bool bVisiblity)
{
	if (ShowOnlyActorsArray.IsEmpty()){return;}
	
	for (AActor* Actor : ShowOnlyActorsArray)
	{
		if (Actor && !Actor->IsPendingKillPending())
		{
			Actor->SetActorHiddenInGame(!bVisiblity);
			
		}
			
	}
	
	
}
