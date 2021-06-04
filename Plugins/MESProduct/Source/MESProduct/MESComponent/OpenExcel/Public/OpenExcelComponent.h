// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <OpenXLSX.h>
#include <SQLiteDatabaseConnection.h>
#include "OpenExcelComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MESPRODUCT_API UOpenExcelComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UOpenExcelComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	

	//UFUNCTION(BlueprintCallable)
		void ImportToDBFromExcel(FString ExcelFullPath, FString DBFullPath);

	UFUNCTION(BlueprintCallable, Category = "MESFunc | Excel")
		void ImportExcelToDB(FString ExcelFullPath, FString ProjectName);
		
private:
	void ProcessExcel(const FString ExcelFullPath, FString DBFullPath);
	void ProcessCsv(const FString CsvFullPath, FString DBFullPath);
	void ImportToDB(TArray<FString> CreateTableSqls, TArray<FString> InsertSqls, FString DBFullPath);
	void ImportToDB(std::vector<std::string> CreateTableSqls, std::vector<std::string> InsertSqls, FString DBFullPath);

	bool CheckDataType(FString CellContent);
	bool CheckDataType(std::string CellContent);

	std::string ConvertToString(FString Str);

	std::vector<std::string> StringSplit(/*const std::string &Source, char Delimiter*/const std::string& input, const std::string& regex);


	FSQLiteDatabaseConnection SqliteConnection;
};
