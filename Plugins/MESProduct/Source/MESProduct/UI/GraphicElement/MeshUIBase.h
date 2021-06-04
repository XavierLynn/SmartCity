// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "Slate/SlateVectorArtData.h"
#include "MeshUIBase.generated.h"

UENUM(BlueprintType)
enum class EGradiantType : uint8
{
	LeftToRight = 0 UMETA(DisplayName = "左->右"),
	UpToDown = 1 UMETA(DisplayName = "上->下"),
	ZeroToFull = 2  UMETA(DisplayName = "0->360度"),
	InToOut = 3 UMETA(DisplayName = "内->外"),
};

/**
 * ZhongBing
 */
UCLASS()
class MESPRODUCT_API UMeshUIBase : public UWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartConfig",Meta = (DisplayName = "渐变方式"))
		EGradiantType GradiantMethod;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartConfig",Meta = (DisplayName = "渐变色列表"))
		TArray<FColor> GradiantColors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartConfig",Meta = (DisplayName = "材质球"))
		UMaterialInterface* Materrial;

public:
	UMeshUIBase();

	//~ Begin UWidget Interface
	virtual void SynchronizeProperties() override;
	//~ End UWidget Interface

	//~ Begin UVisual Interface
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	//~ End UVisual Interface

protected:
	//~ Begin UWidget Interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
	//~ End UWidget Interface

	//interface
	virtual void OnPopulateMesh(const FGeometry& AllottedGeometry,TArray<FSlateVertex>& vertex, TArray<SlateIndex>& index);
	void PushUpdateVertext(const FVector2D& Position, float Scale);
	TArray<FSlateVertex> VertexData;
	TArray<SlateIndex> IndexData;

	//所有图形的渐变
	virtual FColor GetColorByPos(float x, float y);

	//圆形图的渐变
	virtual FColor GetColorByAngle(float startAngle, float EndAngle, float curAngle);

	//圆形图的渐变
	virtual FColor GetColorByRadius(float Outer, float Inner, float CurRadius);

	void GetRectPoint(FSlateVertex verts[],FVector2D p1, FVector2D p2, float lineThickness);
	
	void AddUIVertexQuad(FSlateVertex FourthVertext[]);
	
	float FirstX;//第一个X值
	float FirstY;//第一个Y值
	float Width;//宽度值
	float Height;//高度值
	
private:

	TSharedPtr<class SUIMeshWidget> MyMesh;

	friend class SUIMeshWidget;

	FColor Lerp(FColor a, FColor b, float t);
	
	float GetScale();

#if WITH_EDITOR
public:
	// UWidget interface
	virtual const FText GetPaletteCategory() override;
	// End UWidget interface
#endif
};
