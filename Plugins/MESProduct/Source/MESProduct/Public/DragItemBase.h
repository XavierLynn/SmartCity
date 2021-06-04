// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DragItemBase.generated.h"

class UDragItemBase;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDragEvent, UDragItemBase*, This);

UENUM(BlueprintType)
enum class ECusorType : uint8
{
	None = 0 UMETA(DisplayName = "None"),
	LeftRight = 1 UMETA(DisplayName = "LeftRight"),
	UpDown = 2 UMETA(DisplayName = "UpDown"),
	SouthEast = 3 UMETA(DisplayName = "SouthEast"),
	SouthWest = 4 UMETA(DisplayName = "SouthWest"),
};
/**
 * 
 */
UCLASS()
class MESPRODUCT_API UDragItemBase : public UUserWidget
{
	GENERATED_BODY()

public:
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MES组件属性", Meta = (DisplayName = "鼠标样式类型"))
			ECusorType CursorType;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MES组件属性", Meta = (DisplayName = "XCanMove"))
			bool XCanMove = true;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MES组件属性", Meta = (DisplayName = "YCanMove"))
			bool YCanMove = true;

		UPROPERTY(EditAnywhere, BlueprintAssignable, Category = "MES组件属性", Meta = (DisplayName = "抓动事件"))
			FDragEvent DragEvent;

		UPROPERTY(EditAnywhere, BlueprintAssignable, Category = "MES组件属性", Meta = (DisplayName = "抓动完成事件"))
			FDragEvent DragFinishEvent;

		UPROPERTY(EditAnywhere, BlueprintAssignable, Category = "MES组件属性", Meta = (DisplayName = "抓动点击事件"))
			FDragEvent DragClickEvent;

		UFUNCTION(BlueprintCallable, Category = "MESFunc")
				void RevertDownPosition(bool X,bool Y);

		UFUNCTION(BlueprintCallable, Category = "MESFunc")
				void OnClickMe();
public:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent);
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

private:
	FVector2D  LastDownPosition;
	FVector2D  CurDownPosition;
	bool IsSelected = false;
};
