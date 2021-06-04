// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "TableData.generated.h"

USTRUCT(BlueprintType)
struct FTableRowData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "行数据"))
		TArray<FString> ColumnDatas;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "HeaderName"))
		FString HeaderName;
};

USTRUCT(BlueprintType)
struct FTableFiled
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "字段名"))
	FString FieldName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "字段索引"))
	int FieldIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "字段类型"))
	FString HeaderType;

};

USTRUCT(BlueprintType)
struct FTableData
{
		GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "表头"))
		TArray<FTableFiled> Headers;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "表单数据"))
		TArray<FTableRowData> RowDatas;	
};