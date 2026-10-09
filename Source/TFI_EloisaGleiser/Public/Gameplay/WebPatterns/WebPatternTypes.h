#pragma once

#include "CoreMinimal.h"
#include "WebPatternTypes.generated.h"

UENUM(BlueprintType)
enum class EWebDifficulty : uint8
{
	Easy,
	Medium,
	Hard
};

USTRUCT(BlueprintType)
struct FWebPatternData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName PatternID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EWebDifficulty Difficulty = EWebDifficulty::Easy;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 SilkCost = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 CapturePoints = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RequiredAccuracy = 0.8f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FVector2D> TargetPoints;
};