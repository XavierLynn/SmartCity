// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Engine/Texture2DDynamic.h"
#include "LauncherSaveGame.generated.h"

USTRUCT(BlueprintType)
struct FProjectInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString ProjectName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Width;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Height;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool IsStandalone;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool IsExample;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString CreateTime;
};

/**
 * 
 */
UCLASS()
class MESPRODUCT_API ULauncherSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool IsAutoOpenLastProject;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString ServerAddress;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FProjectInfo> ProjectInfos;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString UserName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Password;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool IsShowLog;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool IsRememberPassword;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool IsOpenLastProject;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString LastProjectName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool LastProjectIsServer;

	UFUNCTION(BlueprintCallable)
		bool CreateProject(FProjectInfo NewProjectInfo);
	UFUNCTION(BlueprintCallable)
		bool IsProjectExist(FString InProjectName);
	UFUNCTION(BlueprintCallable)
		void UpdateUserSettings(bool AutoOpen, FString ServerAddr, bool ShowLog);
	UFUNCTION(BlueprintCallable)
		void UpdateAccount(FString InUserName, FString InPassword);
	UFUNCTION(BlueprintCallable)
		void DeleteProject(FString InProjectName);
	UFUNCTION(BlueprintCallable)
		bool UpdateProjectName(FString OldName, FString NewName);

	UFUNCTION(BlueprintCallable)
		bool SaveToDisk();
	UFUNCTION(BlueprintCallable)
		bool LoadFromDisk();

	UFUNCTION(BlueprintCallable)
		void CopyDBForProject(FString InProjectName, FString InTargetDirectory);

private:
	bool SaveProductData();
	bool SaveProjectData();
	FString ToJsonString();
	bool LoadProductData();
	bool LoadProjectData();
	bool FromJsonString(const FString& JsonString);
};
