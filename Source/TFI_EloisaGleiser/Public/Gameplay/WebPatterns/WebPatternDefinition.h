// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WebPatternTypes.h"
#include "WebPatternDefinition.generated.h"

/**
 * 
 */
UCLASS()
class TFI_ELOISAGLEISER_API UWebPatternDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Web Pattern")
	FWebPatternData PatternData;
};
