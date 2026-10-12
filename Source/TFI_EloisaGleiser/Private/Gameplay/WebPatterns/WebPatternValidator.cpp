#include "Public/Gameplay/WebPatterns/WebPatternValidator.h"


float FWebPatternValidator::DistanceToSegment(const FVector2D& Point, const FVector2D& SegmentStart,const FVector2D& SegmentEnd)
{
	const FVector2D Segment = SegmentEnd - SegmentStart;

	const double LengthSquared = Segment.SizeSquared();

	if (LengthSquared <= UE_SMALL_NUMBER)
	{
		return FVector2D::Distance(Point, SegmentStart);
	}

	const double T = FMath::Clamp(FVector2D::DotProduct(Point - SegmentStart, Segment)/ LengthSquared,0.0,1.0);

	const FVector2D ClosestPoint = SegmentStart + Segment * T;

	return FVector2D::Distance(Point, ClosestPoint);
}

float FWebPatternValidator::CalculateAccuracy(const TArray<FVector2D>& TargetPoints,const TArray<FVector2D>& DrawnPoints)
{
	 if (TargetPoints.Num() < 2 || DrawnPoints.Num() < 2)
    {
        return 0.0f;
    }

    constexpr float Tolerance = 0.04f;
    constexpr int32 SamplesPerSegment = 30;

    // 1. Cobertura del patron objetivo

    int32 CoveredSamples = 0;
    int32 TotalSamples = 0;

    for (int32 i = 0; i < TargetPoints.Num() - 1; ++i)
    {
        const FVector2D Start = TargetPoints[i];
        const FVector2D End = TargetPoints[i + 1];

        for (int32 Sample = 0; Sample <= SamplesPerSegment; ++Sample)
        {
            const float Alpha =static_cast<float>(Sample) / SamplesPerSegment;

            const FVector2D TargetSample =FMath::Lerp(Start, End, Alpha);

            ++TotalSamples;

            for (int32 j = 0; j < DrawnPoints.Num() - 1; ++j)
            {
                const float Distance = DistanceToSegment(TargetSample,DrawnPoints[j],DrawnPoints[j + 1]);

                if (Distance <= Tolerance)
                {
                    ++CoveredSamples;
                    break;
                }
            }
        }
    }

    const float Coverage = TotalSamples > 0? static_cast<float>(CoveredSamples) / TotalSamples: 0.0f;

    // 2. Precisión del trazo del jugador

    int32 AccuratePoints = 0;

    for (const FVector2D& DrawnPoint : DrawnPoints)
    {
        float MinimumDistance = TNumericLimits<float>::Max();

        for (int32 i = 0; i < TargetPoints.Num() - 1; ++i)
        {
            const float Distance = DistanceToSegment(DrawnPoint,TargetPoints[i],TargetPoints[i + 1]);

            MinimumDistance = FMath::Min(MinimumDistance,Distance);
        }

        if (MinimumDistance <= Tolerance)
        {
            ++AccuratePoints;
        }
    }

    const float Precision =static_cast<float>(AccuratePoints) / DrawnPoints.Num();

    // 3. Resultado combinado

    return Coverage * Precision;
}

