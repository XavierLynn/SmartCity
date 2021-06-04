// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "ChartBaseWidget.generated.h"



UENUM(BlueprintType)
enum class EGradiantMethod : uint8
{
	LeftToRight = 0 UMETA(DisplayName = "左->右"),
	UpToDown = 1 UMETA(DisplayName = "上->下"),
	ZeroToFull = 2  UMETA(DisplayName = "0->360度"),
	InToOut = 3 UMETA(DisplayName = "内->外"),
};

UCLASS()
class UECHARTS_API UChartBaseWidget : public UWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartConfig", Meta = (DisplayName = "GradiantStyle"))
		EGradiantMethod GradiantStyle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartConfig", Meta = (DisplayName = "GradientColorList"))
		TArray<FColor> GradiantColors;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartConfig", Meta = (DisplayName = "Material"))
		UMaterialInterface* UIMaterial;

public:
	UChartBaseWidget();


	virtual void SynchronizeProperties();

	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	
protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	virtual void OnPopulateMesh(const FGeometry& AllottedGeometry, TArray<FSlateVertex>& vertex, TArray<SlateIndex>& index);
	void PushUpdateVertext(const FVector2D& Position, float Scale);
	TArray<FSlateVertex> VertexData;

	TArray<SlateIndex> IndexData;


	virtual FColor GetColorByPos(float x, float y);
	//圆形图的渐变
	virtual FColor GetColorByAngle(float startAngle, float EndAngle, float curAngle);

	//圆形图的渐变
	virtual FColor GetColorByRadius(float Outer, float Inner, float CurRadius);
	void GetRectPoint(FSlateVertex verts[], FVector2D p1, FVector2D p2, float lineThickness);
	void GetRectPointLine(FSlateVertex verts[], FVector2D p1, FVector2D p2, float lineThickness, TArray<FSlateVertex> TempPoints, int pos, float AAlineThickness);


	void GetRectPointLineByZhang(FSlateVertex verts[], FVector2D p1, FVector2D p2, float lineThickness, TArray<FSlateVertex> TempPoints, int pos, float AAlineThickness);
	void AddUIVertexQuad(FSlateVertex FourthVertext[]);
	void SetDrawEffect(ESlateDrawEffect InDrawEffects = ESlateDrawEffect::None);
	void Print_C(FString InString);

	float FirstX;//第一个X值
	float FirstY;//第一个Y值
	float Width;//宽度值
	float Height;//高度值
	ESlateDrawEffect DrawEffects = ESlateDrawEffect::None;





private:
	TSharedPtr<class SMyUIMeshWidget> BaseMesh;
	friend class SMyUIMeshWidget;
	FColor Lerp(FColor a, FColor b, float t);
	





#if WITH_EDITOR
public:
	// UWidget interface
	virtual const FText GetPaletteCategory() override;
	// End UWidget interface
#endif
};
