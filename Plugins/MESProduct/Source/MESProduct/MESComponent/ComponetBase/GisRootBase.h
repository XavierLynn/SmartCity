// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "GisRootBase.generated.h"

UENUM(BlueprintType)
enum class EGISType : uint8
{
	MES = 0 UMETA(DisplayName = "MES内置"),
	WGS84 = 1 UMETA(DisplayName = "WGS84"),//无偏坐标系
	GCJ02 = 2 UMETA(DisplayName = "火星"), //国测局坐标 中国专用 google地图在中国用的就是这个坐标系
	BD_09 = 3 UMETA(DisplayName = "百度"),
	GaoDe = 4 UMETA(DisplayName = "高德"),
	GoogleEarth = 5 UMETA(DisplayName = "谷歌的球"),
	Other1 = 6 UMETA(DisplayName = "其他1"),
	Other2 = 7 UMETA(DisplayName = "其他2"),
};

USTRUCT(BlueprintType)
struct FDouble
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Double值"))
		FString StringValue;

	double GetValue()
	{
		NumValue = FCString::Atod(*StringValue);
		return NumValue;
	}

private:
	double NumValue;
};

USTRUCT(BlueprintType)
struct FGISPos
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "坐标类型"))
		EGISType GisType = EGISType::WGS84;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "经度"))
		FDouble Lon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "纬度"))
		FDouble Lat;

	FVector UnrealPos;//虚幻4里面的坐标
};

USTRUCT(BlueprintType)
struct FGisPoint
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "组件定制属性", meta = (DisplayName = "名字"))
		FString Name;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (DisplayName = "Location"))
		FVector Location;//虚幻4里面的坐标

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "组件定制属性", meta = (DisplayName = "坐标列表"))
		TArray<FGISPos> PosList;
		
};
/**
 * 
 */
UCLASS()
class MESPRODUCT_API AGisRootBase : public AStaticMeshActor
{
	GENERATED_BODY()
public:

	TMap<EGISType, TArray<FGISPos>> GisDic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "组件定制属性", meta = (DisplayName = "校准点骨骼"))
		USkeleton* Skeleton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "组件定制属性", meta = (DisplayName = "校准点"))
		TArray<FGisPoint> FixPos;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "组件定制属性", meta = (DisplayName = "缩放"))
		float Scale;
		
	UFUNCTION(BlueprintCallable, Category = "MESFunc")
		void RegisterGisPos();

	UFUNCTION(BlueprintCallable, Category = "MESFunc")
		FVector GetPos(EGISType gtype, float x, float y, float z);

	UFUNCTION(BlueprintCallable, Category = "MESFunc")
		void ClearGisDic();

	UFUNCTION(BlueprintCallable, Category = "MESFunc")
		FString GetRefBoneName(int index);

	UFUNCTION(BlueprintCallable, Category = "MESFunc")
		TArray<FTransform> GetRefTransform();

	UFUNCTION(BlueprintCallable, Category = "MESFunc")
		void SetGisRoot();

	UFUNCTION(BlueprintCallable, Category = "MESFunc")
    void SetScale(float inscale);
	
	FVector GetPosUseDouble(EGISType gtype, double x, double y, double z);

	static AGisRootBase* CurGisRoot;
private:
	FVector CaculatPos(double X, double Y, double Z, TArray<FGISPos> list, int LefIndex);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Overridable function called whenever this actor is being removed from a level */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

#if WITH_EDITOR
public:
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
