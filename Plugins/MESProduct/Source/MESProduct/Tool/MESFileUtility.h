// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MESFileUtility.generated.h"

/**
 * 
 */
UCLASS()
class MESPRODUCT_API UMESFileUtility : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "MESUtil | File"/*, meta = (WorldContext = "WorldContextObject")*/)
		static void OpenFileDialog(FString& OutPath);
	UFUNCTION(BlueprintCallable, Category = "MESUtil | File")
		static void OpenSpecificFileDialog(TArray<FString> FileTypeDescArray, FString& OutPath);

	UFUNCTION(BlueprintCallable, Category = "MESUtil | File")
		static void FindFiles(TArray<FString>& FoundFiles, FString Path, FString FileExtensionIncludeDot, bool IsRecursively);
	UFUNCTION(BlueprintCallable, Category = "MESUtil | File")
		static bool CreateSpecificFile(FString Path, FString FileName, FString FileExtensionIncludeDot);
	// FullPath : Directory Path/FileName.FileExtension
	UFUNCTION(BlueprintCallable, Category = "MESUtil | File")
		static bool CreateFileWithFullPath(FString FileFullPath);

	UFUNCTION(BlueprintCallable, Category = "MESUtil | File")
		static bool OpenConfirmDialog(FString Message);

	UFUNCTION(BlueprintCallable, Category = "MESUtil | File")
		static bool CopyFileFromTo(FString FromPath, FString ToPath);
	UFUNCTION(BlueprintCallable, Category = "MESUtil | File")
		static bool CreateDirectoryAt(FString DirectoryPath);
	UFUNCTION(BlueprintCallable, Category = "MESUtil | File")
		static bool DeleteDic(FString DicPath);
	UFUNCTION(BlueprintCallable, Category = "MESUtil | File")
		static bool SaveStringToPath(FString Content, FString FilePath, FString FileName);
	UFUNCTION(BlueprintCallable, Category = "MESUtil | File")
		static FString ReadFile(FString FilePath, FString FileName);
};
