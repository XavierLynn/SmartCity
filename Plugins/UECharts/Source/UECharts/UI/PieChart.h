// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UECharts/UI/ChartBaseWidget.h"
#include "Slate/SlateVectorArtData.h"

#include "PieChart.generated.h"


USTRUCT(BlueprintType)
struct FPieSeries
{
	GENERATED_USTRUCT_BODY()


	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "PieStyle")
	int32 index = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PieStyle")
	FString Name = "PieItemName";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PieStyle")
	float Value = 15;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PieStyle")
	TArray<FColor> PieGradiantColors = {FColor(255, 255, 255)};
};
UENUM(BlueprintType)


enum class EPieRiders : uint8
{
	Outer = 0 UMETA(DisplayName ="外环"),
	Center = 1 UMETA(DisplayName = "环中心"),
	Inner = 2 UMETA(DisplayName = "内环"),
	Other = 3 UMETA(DisplayName = "其他半径"),
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPieActive, FPieSeries, PieSeries);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPieUnActive);

UCLASS()
class UECHARTS_API UPieChart : public UChartBaseWidget
{
	GENERATED_BODY()

public:

	UPieChart();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PieConfig", Meta = (DisplayName = "环形宽度"))
	float CircleWidth = 10.f; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PieConfig", Meta = (DisplayName = "起始角"))
	float StartAngle = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PieConfig", meta = (DisplayName = "外圆Hover半径"))
	float OutRadisAdd = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PieConfig", meta = (DisplayName = "饼图间隔"))
	float PieInternal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PieConfig", meta = (DisplayName = "饼图样式"))
	TArray<FPieSeries> PieSeries;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PieConfig", Meta = (DisplayName = "环半径范围"))
		EPieRiders PieRiders;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PieConfig", Meta = (DisplayName = "标签字体大小"))
		float LabelFontSize;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PieConfig", meta = (DisplayName = "第一根标签线长"))
	//float FirstLabelLengh;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PieConfig", meta = (DisplayName = "第二根标签线长"))
	//float SceondLabelLengh;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PieConfig", meta = (DisplayName = "标签线宽"))
	//float LabelWith;

	UPROPERTY(BlueprintAssignable)
	FPieActive OnPieHover;
	UPROPERTY(BlueprintAssignable)
	FPieUnActive UnPieHover;


public:

	UFUNCTION(BlueprintCallable, Category = "PieConfig")
		float PieAnimation(const FVector2D InMouseLocationOnViewpot);

	UFUNCTION(BlueprintCallable, Category = "PieConfig")
		void HoverPieItem(const int CurHoverIndex);

	UFUNCTION(BlueprintCallable, Category = "PieConfig")
		TArray<FVector2D> GetAngleCenter();

	//返回每个环不同半径得点
	UFUNCTION(BlueprintCallable, Category = "PieConfig")
		TMap<int, FVector4> GetAllAngleCenterPoints(EPieRiders InPieRiders, float OtherRiders = 0.0f);

	UFUNCTION(BlueprintCallable, Category = "PieConfig")
		void CalculateLabelPositionY(TMap<int, float>& LeftLabelPosition, TMap<int, float>& RightLabelPosition, const TMap<int, FVector4> CurPieCenterPoints, int32 InLabelFontSize);

	UFUNCTION(BlueprintCallable, Category = "PieConfig")
		void CalculateLabelPositionVector(TMap<int, FVector2D>& LeftLabelPosition, TMap<int, FVector2D>& RightLabelPosition, const TMap<int, FVector4> CurPieCenterPoints,const int32 InLabelFontSize, const int32 LabelLineRadisOuterOffset);

	UFUNCTION(BlueprintCallable, Category = "PieConfig")
		void CalculateLabelOffsetVector(const float LabelOffset,float& RightLabelOffset,float& LeftLabelOffset);
	
	UFUNCTION(BlueprintCallable, Category = "PieConfig")
		void ResetAngleValue();

	//UFUNCTION(BlueprintCallable, Category = "PieConfig")
	//	TArray<FVector2D> FindPieElemCenterPoint();

	//绘制标签线
	/*UFUNCTION(BlueprintCallable, Category = "PieConfig")
	void DrawLabelLine(const TMap<int, FVector4> CurAngleCenterPoint,float InFirstLableLengh,float InScendLabelLengh,float InLabelWith);*/

	

protected:
	virtual void OnPopulateMesh(const FGeometry& AllottedGeometry, TArray<FSlateVertex>& vertex,
	                            TArray<SlateIndex>& index) override;

private:
	void DrawCircle(float outer, float inner, float start, float end, float offsetX, float offsetY, bool OuterAnti,
	                bool innerAnti);

private:
	void DataToRadian(TArray<FPieSeries>& InPieData);


	FVector2D CircleCenter;

	float AngleValue = -1;

	FVector2D PieItemRange;

	TMap<int32, float> PieIndex_Angle;
	TArray<FVector2D> AngleCenters;
	float InternalAngle;

	float EndAngle = 360.f;

	int CurIndex = -1;
	bool IsHover = false;
};
