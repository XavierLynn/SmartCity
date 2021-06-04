// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <SQLiteDatabaseConnection.h>
#include "MESProduct\MESComponent\ComponetBase\TableData.h"
#include "SqliteTool.generated.h"

USTRUCT(BlueprintType)
struct FSqliteRowData
{
	GENERATED_BODY()

		UPROPERTY(BlueprintReadWrite)
		TArray<FString> ColumnDatas;
};

USTRUCT(BlueprintType)
struct FSqliteData
{
	GENERATED_BODY()

		UPROPERTY(BlueprintReadWrite)
		TArray<FSqliteRowData> RowDatas;
	UPROPERTY(BlueprintReadWrite)
		TArray<FString> Headers;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MESPRODUCT_API USqliteTool : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USqliteTool();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// select * from 'DataTableName', all the datas, no 'where' limit
	UFUNCTION(BlueprintCallable)
		FSqliteData LoadLocalData(FString DBPath, FString DataTableName);
	UFUNCTION(BlueprintCallable)
		FSqliteData LoadDataWithoutHeader(FString DBPath, FString SelectSqlString);
	UFUNCTION(BlueprintCallable)
		FSqliteData LoadDataIncludeHeader(FString DBPath, FString SelectSqlString, FString DataTableName);

	// effect on 'insert', 'update' and 'delete', except select statement.
	UFUNCTION(BlueprintCallable)
		static bool ExecuteSqlString(FString DBPath, TArray<FString> SqlStrings);

	UFUNCTION(BlueprintCallable)
		TArray<FString> GetDataTableHeader(FString DBPath, FString DataTableName);

	UFUNCTION(BlueprintCallable)
		void CollectData(FString KeyName, FString KeyValue, FString ColumnName, FString NewValue);

	UFUNCTION(BlueprintCallable)
		void SaveData(FString DBPath, FString DataTableName);

	static void LoadDataForCpp(FString DBPath, FString SelectSqlString, FString DataTableName, FSqliteData& Result);
	static TArray<FString> GetDataTableHeaderForCpp(FString DBPath, FString DataTableName);

	static void LoadTableData(FString DBPath, FString SelectSqlString, FString DataTableName, FTableData& Result);

	static void GetColumnName(FString DBPath, FString Sql, TArray<FDatabaseColumnInfo>& ColumnInfos);
private:
	
	static FSQLiteDatabaseConnection SqliteConnect;

	// relative 2 function: CollectData() , SaveData()
	// update datatable set 'ColumnName' = 'NewValue'... where 'KeyName' = 'KeyValue'
	struct FChangeDataInfo
	{
		FString KeyName;
		FString KeyValue;
		FString ColumnName;
		FString NewValue;

		FChangeDataInfo(FString InKeyName, FString InKeyValue, FString InColumnName, FString InNewValue)
		{
			KeyName = InKeyName;
			KeyValue = InKeyValue;
			ColumnName = InColumnName;
			NewValue = InNewValue;
		}
	};

	static TMap<FString, TArray<FChangeDataInfo>> ChangedDatas;

	static void LogSqliteError(FString SqlString);
};
