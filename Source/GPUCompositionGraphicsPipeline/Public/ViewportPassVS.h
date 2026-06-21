// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */

// The vertex shader used by DrawScreenPass to draw a rectangle.
class FViewportPassVS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FViewportPassVS);

	FViewportPassVS() = default;
	FViewportPassVS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{}
};