// Fill out your copyright notice in the Description page of Project Settings.


#include "DragItemBase.h"
#include "ApplicationCore/Public/GenericPlatform/ICursor.h"

void UDragItemBase::RevertDownPosition(bool X, bool Y)
{
	if (X == true) {
		CurDownPosition.X = LastDownPosition.X;
	}
	if (Y == true) {
		CurDownPosition.Y = LastDownPosition.Y;
	}
}

void UDragItemBase::OnClickMe()
{
	FGeometry Gemo = GetCachedGeometry();
	FPointerEvent pointerEvent;
	NativeOnMouseButtonDown(Gemo, pointerEvent);
}

FReply UDragItemBase::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	IsSelected = true;
	float MouseX;
	float MouseY;
	GetOwningPlayer()->GetMousePosition(MouseX, MouseY);
	const FGeometry& Geomotry = GetParent()->GetCachedGeometry();
	CurDownPosition = Geomotry.AbsoluteToLocal(FVector2D(MouseX, MouseY));
	DragClickEvent.Broadcast(this);
	return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
}

FReply UDragItemBase::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	IsSelected = false;
	DragFinishEvent.Broadcast(this);
	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

void UDragItemBase::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	APlayerController* p = GetOwningPlayer();
	if (p != nullptr)
	{
		switch (CursorType)
		{
		case ECusorType::LeftRight:
			p->CurrentMouseCursor = EMouseCursor::ResizeLeftRight;
			break;
		case ECusorType::UpDown:
			p->CurrentMouseCursor = EMouseCursor::ResizeUpDown;
			break;
		case ECusorType::SouthEast:
			p->CurrentMouseCursor = EMouseCursor::ResizeSouthEast;
			break;
		case ECusorType::SouthWest:
			p->CurrentMouseCursor = EMouseCursor::ResizeSouthWest;
			break;
		}
	}
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
}

void UDragItemBase::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	APlayerController* p = GetOwningPlayer();
	if (p != nullptr)
	{
		p->CurrentMouseCursor = EMouseCursor::Default;
	}
	Super::NativeOnMouseLeave(InMouseEvent);
}

void UDragItemBase::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	if (IsSelected)
	{
		if (!FSlateApplication::Get().GetPressedMouseButtons().Contains(EKeys::LeftMouseButton))
		{
			FPointerEvent pointerEvent;
			NativeOnMouseButtonUp(MyGeometry, pointerEvent);
		}
		float MouseX;
		float MouseY;
		if (!GetOwningPlayer()->GetMousePosition(MouseX, MouseY))
			return;
		UCanvasPanelSlot* s = Cast<UCanvasPanelSlot>(Slot);
		if (s != nullptr)
		{
			const FGeometry& Geomotry = GetParent()->GetCachedGeometry();
			LastDownPosition = CurDownPosition;
			CurDownPosition = Geomotry.AbsoluteToLocal(FVector2D(MouseX, MouseY));
			if (!XCanMove)
			{
				CurDownPosition.X = LastDownPosition.X;
			}
			if (!YCanMove)
			{
				CurDownPosition.Y = LastDownPosition.Y;
			}
			s->SetPosition(s->GetPosition() + CurDownPosition - LastDownPosition);
			DragEvent.Broadcast(this);
		}
	}
	Super::NativeTick(MyGeometry, InDeltaTime);
}
