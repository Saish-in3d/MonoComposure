// Copyright Epic Games, Inc. All Rights Reserved.

#include "GPUCompositionGraphicsPipeline.h"

#define LOCTEXT_NAMESPACE "FGPUCompositionGraphicsPipelineModule"

void FGPUCompositionGraphicsPipelineModule::StartupModule()
{
	
	FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("GPUCompositionGraphicsPipeline"))->GetBaseDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/Plugins/GPUCompositionGraphicsPipeline"), PluginShaderDir);
}

void FGPUCompositionGraphicsPipelineModule::ShutdownModule()
{
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FGPUCompositionGraphicsPipelineModule, GPUCompositionGraphicsPipeline)