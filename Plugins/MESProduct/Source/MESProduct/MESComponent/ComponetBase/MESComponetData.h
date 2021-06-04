// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MESProduct\MESComponent\ComponetBase\TableData.h"
#include "UMG\Public\Components\Widget.h"
#include "UMG\Public\Blueprint\UserWidget.h"
#include "MESComponetData.generated.h"

UENUM(BlueprintType)
enum class EDataSourceType : uint8
{
	NONE,
	SQLite,
	API,
	DAC
};

UENUM(BlueprintType)
enum class EDataSortType : uint8
{
	Up = 0 UMETA(DisplayName = "升序"),
	Down = 1 UMETA(DisplayName = "降序")
};

USTRUCT(BlueprintType)
struct FDataSort
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "表字段"))
		FString ColumnName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "排序方式"))
		EDataSortType SortType;
};


UENUM(BlueprintType)
enum class EFilterType : uint8
{
	equal = 0 UMETA(DisplayName = "="),
	greater = 1 UMETA(DisplayName = ">"),
	greater_equal = 2 UMETA(DisplayName = ">="),
	less = 3 UMETA(DisplayName = "<"),
	less_equal = 4 UMETA(DisplayName = "<="),
	in = 5 UMETA(DisplayName = "in"),
};

UENUM(BlueprintType)
enum class EDataBindType : uint8
{
	FiledName = 0 UMETA(DisplayName = "表字段名"),
	FiledIndex = 1 UMETA(DisplayName = "字段索引"),
	FiledArea = 2 UMETA(DisplayName = "字段区间"),
	FiledNameList = 3 UMETA(DisplayName = "字段列表"),
};

USTRUCT(BlueprintType)
struct FFilterRule
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "字段名"))
		FString ColumnName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "条件"))
		EFilterType FilterType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "值"))
		FString FilterValue;
};

USTRUCT(BlueprintType)
struct FDataBind
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "绑定方式"))
		EDataBindType BindType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "值"))
		FString FilterValue;
};

class AMESActorBase;
class UMESComponetBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateDataDelegate, FTableData, data);
DECLARE_DYNAMIC_DELEGATE_OneParam(FUpdateDataSingleDelegate, FTableData, data);

UCLASS(BlueprintType, Blueprintable, config = Engine, meta = (DontUseGenericSpawnObject = "True"))
class MESPRODUCT_API UMESComponetData : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MES组件属性")
		EDataSourceType DataSourceType = EDataSourceType::NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MES组件属性")
	FString DataGroup;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MES组件属性")
		FString TableName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "MES组件属性")
		TMap<FString, FDataBind> DataBinds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "MES组件属性")
		TArray<FFilterRule> FiterRules;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "MES组件属性")
		TArray<FDataSort> Sorts;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "MES组件属性")
		int MaxNum = 5000;
	//=================
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "MES本地属性")
		FTableData TableData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "MES组件属性")
		AMESActorBase* actorBase;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "MES组件属性")
		UMESComponetBase* UIBase;

	UFUNCTION(BlueprintCallable, Category = "MESFunc")
		void BindGetComponetData(FUpdateDataSingleDelegate CallBack);

	UFUNCTION(BlueprintCallable, Category = "MESFunc")
		void UnBindGetComponetData(FUpdateDataSingleDelegate CallBack);

	UFUNCTION(BlueprintCallable, Category = "MESFunc")
		void SetFinish();

	UFUNCTION(BlueprintCallable, Category = "MESFunc")
		void SaveDataToComponet();

	UFUNCTION(BlueprintCallable, Category = "MESFunc")
		void FetechDataTable();
};
