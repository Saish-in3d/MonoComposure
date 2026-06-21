// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "CommonStructs.h"
#include "CompGraphicsSubsystem.generated.h"

class UMediaTexture;
class UTextureRenderTarget2D;

/**
 * 
 */
UCLASS()
class GPUCOMPOSITIONGRAPHICSPIPELINE_API UCompGraphicsSubsystem : public UEngineSubsystem
{
	GENERATED_BODY() 
	
public:
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	UFUNCTION(BlueprintCallable,Category = "GPU Composition")
	void InitilizeCompositor();
	
	UFUNCTION(BlueprintCallable,Category = "GPU Composition")
	void StartCompositor();	
	
	UFUNCTION(BlueprintCallable,Category = "GPU Composition")
	void EndCompositor();
	
	UFUNCTION(BlueprintCallable,Category = "GPU Composition")
	bool GetShouldCompositorRun();
	
	UFUNCTION(BlueprintCallable,Category = "GPU Composition")
	void SetInputMediaTexture(UMediaTexture* inMediaTexture, bool TransparentMediaTexture);
	
	//Returns a render target that the final composited output
	UFUNCTION(BlueprintCallable,Category = "GPU Composition")
	UTextureRenderTarget2D* GetCompositionOutputRenderTarget();
	
	UFUNCTION(BlueprintCallable,Category = "GPU Composition")
	void ConfigureCompositorOutput(FCompositorOutputSpecs inCompositorOutputSpecs);
	
	UFUNCTION(BlueprintCallable,Category = "GPU Composition")
	void DeinitializeCompositor();
	
	UFUNCTION(BlueprintCallable,Category = "GPU Composition")
	void SetShowOnlyActors(TArray<AActor*> inActorsArray);
	
	UFUNCTION(BlueprintCallable,Category = "GPU Composition")
	void ResetShowOnlyActors();
	
	UFUNCTION(BlueprintCallable,Category = "GPU Composition")
	void AddShowOnlyActor(AActor* inActor);
	
	UFUNCTION(BlueprintCallable,Category = "GPU Composition")
	void RemoveShowOnlyActor(AActor* inActor);
	
	UFUNCTION(BlueprintCallable,Category = "GPU Composition")
	void SetCGOverlayonBackgroundFootageValue(float Value);
	
	UFUNCTION(BlueprintCallable,Category = "GPU Composition")
	void SetVirtualWorldOverlayValue(float Value);
	
	UFUNCTION(BlueprintCallable,Category = "GPU Composition")
    void SetCompositionResolution(float X, float Y);
	
	UFUNCTION(BlueprintCallable,Category = "GPU Composition")
	float GetCGOverlayonBackgroundFootageValue();
	
	UFUNCTION(BlueprintCallable,Category = "GPU Composition")
	float GetVirtualWorldOverlayValue();
	
	UFUNCTION(BlueprintCallable,Category = "GPU Composition")
	bool GetShallCompOutputOnViewport();
	
	UFUNCTION(BlueprintCallable,Category = "GPU Composition")
	void ToggleCompOutputOnViewport(bool inShallCompOutputOnViewport);
	
private:
	
	//Private Variables
	TSharedPtr<class FCoreSVE, ESPMode::ThreadSafe> CoreSVE;
	
	UPROPERTY()
	UTextureRenderTarget2D* OutputRenderTarget;
	
	UPROPERTY()
	UMediaTexture* MediaTexture;
	
	UPROPERTY()
	FCompositorOutputSpecs _CompositorOutputSpecs;
	
	UPROPERTY()
	TArray<AActor*> ShowOnlyActorsArray;
	
	UPROPERTY()
	float CGOverlayonBackgroundFootageValue = 1.f;
	
	UPROPERTY()
	float VirtualWorldOverlayValue = 1.f;
	
	UPROPERTY()
	bool bShallCompOutputOnViewport = 1.f;
	
	UPROPERTY()
	bool bShouldCompositorRun;
	
	UPROPERTY()
	bool bUseTransparentMediaTexture;
	
	
	//Private functions
	UTextureRenderTarget2D* CreateRenderTarget(FCompositorOutputSpecs inCompositorOutputSpecs);
	
	void CopyTexture2DtoRenderTarget(UTextureRenderTarget2D* DestinationRenderTarget, UTexture2D* SourceTexture );
	
	void ProcessShowOnlyActors();
	
	void SetVisiblityOfAllActors(bool bVisiblity);
	
	void SetVisiblityShowOnlyActors(bool bVisiblity);
	
	
	
	
public: //C++ functions
	UMediaTexture* GetImputMediaTextureInternal();
	
	UTextureRenderTarget2D* GetCompositionOutputRenderTargetInternal();
	
	bool GetShallUseTransparentMediaTexture();
	
};
