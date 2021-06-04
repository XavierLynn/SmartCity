// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MESComponetBase.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "MESProduct\MESComponent\ComponetBase\MESActorBase.h"
#include "MESProduct\MESComponent\ComponetBase\MESPointComponetBase.h"
#include "MESStyleBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FClickCallBack, UMESStyleBase*, style);
/**
 * 
 */
UCLASS()
class MESPRODUCT_API UMESStyleBase : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "样式属性", Meta = (DisplayName = "ComponetType"))
		EMESComponetType ComponetType = EMESComponetType::UIComponet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "样式属性", Meta = (DisplayName = "DragOffset"))
		FVector2D DragOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "样式属性", Meta = (DisplayName = "UI组件"))
		TSubclassOf<UMESComponetBase> DragComponet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "样式属性", Meta = (DisplayName = "3D组件"))
		TSubclassOf<AMESActorBase> DragActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "样式属性", Meta = (DisplayName = "打点组件"))
		TSubclassOf<AMESPointComponetBase> DragPointComponet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "样式属性", Meta = (DisplayName = "选中框"))
		UWidget* SelectFrame;

	UPROPERTY(BlueprintReadWrite, Category = "样式属性", Meta = (DisplayName = "IsDraging"))
		bool IsDraging = false;

	UPROPERTY(BlueprintReadWrite, Category = "样式属性")
		FString TextName;

	UPROPERTY(BlueprintAssignable)
		FClickCallBack OnClickCallBack;
public:
	//从UserWidget继承下俩
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual bool Initialize() override;
private:
	static TMap<uint32, TArray<UMESStyleBase*>> DicStyle;
	int ParentUUID;
};
