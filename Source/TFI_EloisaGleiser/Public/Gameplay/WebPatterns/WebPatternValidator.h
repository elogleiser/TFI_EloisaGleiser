#pragma once

#include "CoreMinimal.h"

class FWebPatternValidator
{
public:

	static float CalculateAccuracy(const TArray<FVector2D>& TargetPoints,const TArray<FVector2D>& DrawnPoints);
	
private:

	static float DistanceToSegment(const FVector2D& Point,const FVector2D& SegmentStart,const FVector2D& SegmentEnd);
};