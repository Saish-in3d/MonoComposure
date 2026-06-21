// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PathBPFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class GPUCOMPOSITIONGRAPHICSPIPELINE_API UPathBPFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Paths")
	static FString GameContentDir() { return FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()); }
	
};