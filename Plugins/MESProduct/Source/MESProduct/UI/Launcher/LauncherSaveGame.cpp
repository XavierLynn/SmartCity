// Fill out your copyright notice in the Description page of Project Settings.


#include "LauncherSaveGame.h"
//#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/PreWindowsApi.h"
#include "Windows/PostWindowsApi.h"

#include "Kismet/GameplayStatics.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "JsonUtilities/Public/JsonUtilities.h"
#include "HAL/FileManager.h"
#include "MESProduct/Tool/MESPathUtility.h"
#include "GenericPlatform/GenericPlatformFile.h"

bool ULauncherSaveGame::CreateProject(FProjectInfo NewProjectInfo)
{
	FString NewProjectName = NewProjectInfo.ProjectName;
	if (IsProjectExist(NewProjectName))
	{
		return false;
	}

	bool Result = true;
	ProjectInfos.Add(NewProjectInfo);

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	FString BasePath = UMESPathUtility::GetProjectBasePath();
	FString ConfigPath = BasePath + TEXT("单机\\") + NewProjectName + TEXT("\\配置\\");
	PlatformFile.CreateDirectoryTree(*ConfigPath);

	FString ConfigFullPath = ConfigPath + TEXT("项目配置.json");
	FString JsonString;
	FJsonObjectConverter::UStructToJsonObjectString(NewProjectInfo, JsonString);
	bool SaveFileResult = FFileHelper::SaveStringToFile(JsonString, *ConfigFullPath);
	if (!SaveFileResult)
	{
		UE_LOG(LogTemp, Warning, TEXT("save project data failed, ProjectName: %s, JsonString: %s"), *NewProjectName, *JsonString);
		Result = false;
	}

	CopyDBForProject(NewProjectName, UMESPathUtility::GetStandaloneProjectDataPath(NewProjectName));

	return Result;
}

bool ULauncherSaveGame::IsProjectExist(FString InProjectName)
{
	for (FProjectInfo ProjectInfo : ProjectInfos)
	{
		FString ProjectName = ProjectInfo.ProjectName;
		if (ProjectName.Equals(InProjectName))
		{
			return true;
		}
	}

	return false;
}

void ULauncherSaveGame::UpdateUserSettings(bool AutoOpen, FString ServerAddr, bool ShowLog)
{
	IsAutoOpenLastProject = AutoOpen;
	ServerAddress = ServerAddr;
	IsShowLog = ShowLog;
}

void ULauncherSaveGame::UpdateAccount(FString InUserName, FString InPassword)
{
	UserName = InUserName;
	Password = InPassword;
}

void ULauncherSaveGame::DeleteProject(FString InProjectName)
{
	for (int i = 0; i< ProjectInfos.Num(); ++i)
	{
		if (ProjectInfos[i].ProjectName.Equals(InProjectName))
		{
			ProjectInfos.RemoveAt(i);

			IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
			FString BasePath = UMESPathUtility::GetProjectBasePath();

			FString ProjectPath = BasePath + TEXT("单机\\") + InProjectName;
			PlatformFile.DeleteDirectoryRecursively(*ProjectPath);
			return;
		}
	}
}

bool ULauncherSaveGame::UpdateProjectName(FString OldName, FString NewName)
{
	bool Result = true;
	for (FProjectInfo& ProjectInfo : ProjectInfos)
	{
		if (ProjectInfo.ProjectName.Equals(OldName))
		{
			ProjectInfo.ProjectName = NewName;

			IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
			FString BasePath = UMESPathUtility::GetProjectBasePath();

			FString OldProjectPath = BasePath + TEXT("单机\\") + OldName;
			FString NewProjectPath = BasePath + TEXT("单机\\") + NewName;
			PlatformFile.CopyDirectoryTree(*NewProjectPath, *OldProjectPath, true);

			FString ConfigFullPath = NewProjectPath + TEXT("\\配置\\") + TEXT("项目配置.json");
			FString JsonString;
			FJsonObjectConverter::UStructToJsonObjectString(ProjectInfo, JsonString);
			bool SaveFileResult = FFileHelper::SaveStringToFile(JsonString, *ConfigFullPath);
			if (!SaveFileResult)
			{
				UE_LOG(LogTemp, Warning, TEXT("save project data failed, ProjectName: %s, JsonString: %s"), *NewName, *JsonString);
				Result = false;
			}

			Result &= PlatformFile.DeleteDirectoryRecursively(*OldProjectPath);
		}
	}
	return Result;
}

bool ULauncherSaveGame::SaveToDisk()
{
	bool Result = true;

	Result &= SaveProductData();
	//Result &= SaveProjectData();

	return Result;
}

bool ULauncherSaveGame::LoadFromDisk()
{
	bool Result = true;

	Result &= LoadProductData();
	Result &= LoadProjectData();

	return Result;
}

void ULauncherSaveGame::CopyDBForProject(FString InProjectName, FString InTargetDirectory)
{
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (!PlatformFile.DirectoryExists(*InTargetDirectory))
	{
		PlatformFile.CreateDirectoryTree(*InTargetDirectory);
	}

	FString ConfigDBName = TEXT("配置数据库.db");
	FString ConfigSourceDBPath = UMESPathUtility::GetProductDataPath() + ConfigDBName;
	FString ConfigTargetDBPath = InTargetDirectory + ConfigDBName;
	if (!PlatformFile.FileExists(*ConfigTargetDBPath))
	{
		PlatformFile.CopyFile(*ConfigTargetDBPath, *ConfigSourceDBPath, EPlatformFileRead::AllowWrite, EPlatformFileWrite::AllowRead);
	}

	FString SimulateDBName = TEXT("模拟数据.db");
	FString SimulateSourceDBPath = UMESPathUtility::GetProductDataPath() + SimulateDBName;
	FString SimulateTargetDBPath = InTargetDirectory + SimulateDBName;
	if (!PlatformFile.FileExists(*SimulateTargetDBPath))
	{
		PlatformFile.CopyFile(*SimulateTargetDBPath, *SimulateSourceDBPath, EPlatformFileRead::AllowWrite, EPlatformFileWrite::AllowRead);
	}
}

bool ULauncherSaveGame::SaveProductData()
{
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	FString BasePath = UMESPathUtility::GetProductBasePath();
	PlatformFile.CreateDirectoryTree(*BasePath);

	FString ConfigPath = BasePath + TEXT("配置\\");
	PlatformFile.CreateDirectoryTree(*ConfigPath);

	FString ConfigFullPath = ConfigPath + TEXT("产品配置.json");
	FString JsonString = ToJsonString();
	if (!FFileHelper::SaveStringToFile(JsonString, *ConfigFullPath))
	{
		UE_LOG(LogTemp, Warning, TEXT("save product data failed, JsonString: %s"), *JsonString);
		return false;
	}
	return true;
}

bool ULauncherSaveGame::SaveProjectData()
{
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	FString BasePath = UMESPathUtility::GetProjectBasePath();
	PlatformFile.CreateDirectoryTree(*BasePath);

	FString StandalonePath = BasePath + TEXT("单机\\");
	FString NetPath = BasePath + TEXT("网络\\");
	//PlatformFile.DeleteDirectoryRecursively(*StandalonePath);
	//PlatformFile.DeleteDirectoryRecursively(*NetPath);

	bool Result = true;
	for (FProjectInfo& ProjectInfo : ProjectInfos)
	{
		FString ProjectName = ProjectInfo.ProjectName;
		if (ProjectName.IsEmpty())
		{
			continue;
		}

		FString TypePath = ProjectInfo.IsStandalone ? TEXT("单机\\") : TEXT("网络\\");

		FString ConfigPath = BasePath + TypePath + ProjectName + TEXT("\\配置\\");
		PlatformFile.CreateDirectoryTree(*ConfigPath);

		FString ConfigFullPath = ConfigPath + TEXT("项目配置.json");
		FString JsonString;
		FJsonObjectConverter::UStructToJsonObjectString(ProjectInfo, JsonString);
		bool SaveFileResult = FFileHelper::SaveStringToFile(JsonString, *ConfigFullPath);
		if (!SaveFileResult)
		{
			Result = false;
			UE_LOG(LogTemp, Warning, TEXT("save project data failed, ProjectName: %s, JsonString: %s"), *ProjectName, *JsonString);
			continue;
		}
	}

	return Result;
}

FString ULauncherSaveGame::ToJsonString()
{
	FString JsonOutString;
	TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> Writer = TJsonWriterFactory< TCHAR, TPrettyJsonPrintPolicy<TCHAR> >::Create(&JsonOutString);
	Writer->WriteObjectStart();

	Writer->WriteValue("IsAutoOpenLastProject", IsAutoOpenLastProject);
	Writer->WriteValue("ServerAddress", ServerAddress);
	Writer->WriteValue("UserName", UserName);
	Writer->WriteValue("Password", Password);
	Writer->WriteValue("IsShowLog", IsShowLog);
	Writer->WriteValue("IsRememberPassword", IsRememberPassword);
	Writer->WriteValue("IsOpenLastProject", IsOpenLastProject);
	Writer->WriteValue("LastProjectName", LastProjectName); 
	Writer->WriteValue("LastProjectIsServer", LastProjectIsServer);
	Writer->WriteObjectEnd();
	Writer->Close();

	return JsonOutString;
}

bool ULauncherSaveGame::LoadProductData()
{
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	FString BasePath = UMESPathUtility::GetProductBasePath();
	FString ConfigFullPath = BasePath + TEXT("配置\\") + TEXT("产品配置.json");
	FString JsonString;
	if (!FFileHelper::LoadFileToString(JsonString, *ConfigFullPath))
	{
		UE_LOG(LogTemp, Warning, TEXT("load product data failed"));
		return false;
	}
	
	return FromJsonString(JsonString);
}

bool ULauncherSaveGame::LoadProjectData()
{
	ProjectInfos.Empty();

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	FString BasePath = UMESPathUtility::GetProjectBasePath();
	TArray<FString> FileRelativePaths;
	PlatformFile.FindFilesRecursively(FileRelativePaths, *BasePath, TEXT(".json"));
	// Files[0] = ../../../../../../work/UnrealProjects/C01_MES_Editor/\StreamingAssets\MESProject\仿真展示/配置/项目配置.json

	const FString TargetJsonFile = TEXT("项目配置.json");
	for (FString FileRelativePath : FileRelativePaths)
	{
		FString FileName = FPaths::GetCleanFilename(FileRelativePath);
		if (FileName.Equals(TargetJsonFile))
		{
			FString JsonString;
			if (!FFileHelper::LoadFileToString(JsonString, *FileRelativePath))
			{
				UE_LOG(LogTemp, Warning, TEXT("load project data failed"));
				continue;
			}
			if (JsonString.IsEmpty())
			{
				continue;
			}

			FProjectInfo LoadProjectInfo;
			FJsonObjectConverter::JsonObjectStringToUStruct<FProjectInfo>(JsonString, &LoadProjectInfo, 0, 0);

			ProjectInfos.Add(LoadProjectInfo);
		}
	}

	
	return false;
}

bool ULauncherSaveGame::FromJsonString(const FString& JsonString)
{
	TSharedRef< TJsonReader<> > Reader = TJsonReaderFactory<>::Create(JsonString);
	TSharedPtr<FJsonObject> JsonObject;
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		IsAutoOpenLastProject = JsonObject->GetBoolField("IsAutoOpenLastProject");
		ServerAddress = JsonObject->GetStringField("ServerAddress");
		UserName = JsonObject->GetStringField("UserName");
		Password = JsonObject->GetStringField("Password");
		IsShowLog = JsonObject->GetBoolField("IsShowLog");
		IsRememberPassword = JsonObject->GetBoolField("IsRememberPassword");
		IsOpenLastProject = JsonObject->GetBoolField("IsOpenLastProject");
		LastProjectName = JsonObject->GetStringField("LastProjectName");
		LastProjectIsServer = JsonObject->GetBoolField("LastProjectIsServer");
		return true;
	}
	return false;
}
