// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WebsDrawingCanvas.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Input/Reply.h"
#include "Rendering/DrawElements.h"



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
	if (InMouseEvent.GetEffectingButton() != EKeys::LeftMouseButton)
	{
		return FReply::Unhandled();
	}

	bIsDrawing = false;

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
	
	if (DrawnPoints.Num() >= 2)
	{
		FSlateDrawElement::MakeLines(
			OutDrawElements,
			ParentLayer + 1,
			AllottedGeometry.ToPaintGeometry(),
			DrawnPoints,
			ESlateDrawEffect::None,
			FLinearColor::White,
			true,
			4.0f
		);
	}

	return ParentLayer + 1;
}