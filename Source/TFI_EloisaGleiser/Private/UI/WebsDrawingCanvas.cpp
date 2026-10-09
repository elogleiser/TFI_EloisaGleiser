// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WebsDrawingCanvas.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Input/Reply.h"
#include "Rendering/DrawElements.h"


void UWebsDrawingCanvas::SetPattern(UWebPatternDefinition* NewPattern)
{
	CurrentPattern = NewPattern;

	ClearDrawing();

	Invalidate(EInvalidateWidgetReason::Paint);
}

float UWebsDrawingCanvas::CalculateAccuracy() const
{
	 if (!CurrentPattern || DrawnPoints.Num() < 2)
    {
        return 0.0f;
    }

    const TArray<FVector2D>& TargetPoints =
        CurrentPattern->PatternData.TargetPoints;

    if (TargetPoints.Num() < 2)
    {
        return 0.0f;
    }

    const FVector2D CanvasSize = GetCachedGeometry().GetLocalSize();

    if (CanvasSize.X <= 0.0 || CanvasSize.Y <= 0.0)
    {
        return 0.0f;
    }

    TArray<FVector2D> NormalizedDrawnPoints;

    for (const FVector2D& Point : DrawnPoints)
    {
        NormalizedDrawnPoints.Add(FVector2D(Point.X / CanvasSize.X,Point.Y / CanvasSize.Y));
    }

    constexpr float Tolerance = 0.04f;
    constexpr int32 SamplesPerSegment = 30;

    // 1. Cobertura del patrón objetivo

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

            for (int32 j = 0; j < NormalizedDrawnPoints.Num() - 1; ++j)
            {
                const float Distance = DistanceToSegment(TargetSample,NormalizedDrawnPoints[j],NormalizedDrawnPoints[j + 1]);

                if (Distance <= Tolerance)
                {
                    ++CoveredSamples;
                    break;
                }
            }
        }
    }

    const float Coverage =
        TotalSamples > 0
            ? static_cast<float>(CoveredSamples) / TotalSamples
            : 0.0f;

    // 2. Precision del trazo del jugador

    int32 AccuratePoints = 0;

    for (const FVector2D& DrawnPoint : NormalizedDrawnPoints)
    {
        float MinimumDistance = TNumericLimits<float>::Max();

        for (int32 i = 0; i < TargetPoints.Num() - 1; ++i)
        {
            const float Distance = DistanceToSegment(
                DrawnPoint,
                TargetPoints[i],
                TargetPoints[i + 1]
            );

            MinimumDistance = FMath::Min(MinimumDistance, Distance);
        }

        if (MinimumDistance <= Tolerance)
        {
            ++AccuratePoints;
        }
    }

    const float Precision =
        static_cast<float>(AccuratePoints)
        / NormalizedDrawnPoints.Num();

    // 3. Resultado combinado

    return Coverage * Precision;
}

float UWebsDrawingCanvas::GetRequiredAccuracy() const
{
	if (!CurrentPattern)
	{
		return 1.0f;
	}

	return CurrentPattern->PatternData.RequiredAccuracy;
}

FReply UWebsDrawingCanvas::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() != EKeys::LeftMouseButton)
	{
		return FReply::Unhandled();
	}

	bIsDrawing = true;
	DrawnPoints.Empty();

	const FVector2D LocalPosition =InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());

	DrawnPoints.Add(LocalPosition);

	return FReply::Handled().CaptureMouse(TakeWidget());
}

FReply UWebsDrawingCanvas::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (!bIsDrawing)
	{
		return FReply::Unhandled();
	}

	const FVector2D LocalPosition =InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());

	if (DrawnPoints.IsEmpty() ||FVector2D::Distance(DrawnPoints.Last(),LocalPosition) > 3.0f)
	{
		DrawnPoints.Add(LocalPosition);

		Invalidate(EInvalidateWidgetReason::Paint);
	}

	return FReply::Handled();
}

FReply UWebsDrawingCanvas::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() != EKeys::LeftMouseButton
	   || !bIsDrawing)
	{
		return FReply::Unhandled();
	}

	bIsDrawing = false;

	const float Accuracy = CalculateAccuracy();

	OnDrawingFinished.Broadcast(Accuracy);

	return FReply::Handled().ReleaseMouseCapture();
}

void UWebsDrawingCanvas::ClearDrawing()
{
	DrawnPoints.Empty();
	bIsDrawing = false;

	Invalidate(EInvalidateWidgetReason::Paint);
}

int32 UWebsDrawingCanvas::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId,const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	const int32 ParentLayer = Super::NativePaint(Args,AllottedGeometry,MyCullingRect,OutDrawElements,LayerId,InWidgetStyle,bParentEnabled);
	
	if (CurrentPattern)
	{
		const TArray<FVector2D>& TargetPoints =
			CurrentPattern->PatternData.TargetPoints;

		if (TargetPoints.Num() >= 2)
		{
			TArray<FVector2D> LocalTargetPoints;

			const FVector2D CanvasSize =
				AllottedGeometry.GetLocalSize();

			for (const FVector2D& Point : TargetPoints)
			{
				LocalTargetPoints.Add(
					FVector2D(
						Point.X * CanvasSize.X,
						Point.Y * CanvasSize.Y
					)
				);
			}

			FSlateDrawElement::MakeLines(
				OutDrawElements,
				ParentLayer + 1,
				AllottedGeometry.ToPaintGeometry(),
				LocalTargetPoints,
				ESlateDrawEffect::None,
				FLinearColor(0.1f, 0.7f, 0.6f, 0.5f),
				true,
				5.0f
			);
		}
	}
	
	if (DrawnPoints.Num() >= 2)
	{
		FSlateDrawElement::MakeLines(
			OutDrawElements,
			ParentLayer + 2,
			AllottedGeometry.ToPaintGeometry(),
			DrawnPoints,
			ESlateDrawEffect::None,
			FLinearColor::White,
			true,
			4.0f
		);
	}

	return ParentLayer + 2;
}

float UWebsDrawingCanvas::DistanceToSegment(const FVector2D& Point, const FVector2D& SegmentStart,
	const FVector2D& SegmentEnd)
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
