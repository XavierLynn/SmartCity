// Fill out your copyright notice in the Description page of Project Settings.


#include "MESFileUtility.h"
#include "Misc/Paths.h"
#include "DesktopPlatform/Public/IDesktopPlatform.h"
#include "DesktopPlatform/Public/DesktopPlatformModule.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/FileHelper.h"
#include "Windows.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "Widgets/SWindow.h"
#include "GenericPlatform/GenericWindow.h"
#include "Serialization/JsonWriter.h"

void UMESFileUtility::OpenFileDialog(FString& OutPath)
{
	TArray<FString> FileTypes;
	return OpenSpecificFileDialog(FileTypes, OutPath);
}

void UMESFileUtility::OpenSpecificFileDialog(TArray<FString> FileTypeDescArray, FString& OutPath)
{
	OutPath.Empty();
	TArray<FString> OpenFileNames;
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();

	FString FileType;
	for (FString& FileTypeDesc : FileTypeDescArray)
	{
		FileType += FileTypeDesc + "|";
	}
	if (FileType.Len() > 0)
	{
		FileType = FileType.Left(FileType.Len() - 1);
	}
	
	if (DesktopPlatform->OpenFileDialog(nullptr, TEXT(""), TEXT(""), TEXT(""), FileType, EFileDialogFlags::None, OpenFileNames))
	{
		if (OpenFileNames.Num() > 0)
		{
			OutPath = FPaths::ConvertRelativePathToFull(OpenFileNames[0]);
		}
	}
}

void UMESFileUtility::FindFiles(TArray<FString>& FoundFiles, FString Path, FString FileExtensionIncludeDot, bool IsRecursively)
{
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (IsRecursively)
	{
		PlatformFile.FindFilesRecursively(FoundFiles, *Path, *FileExtensionIncludeDot);
	}
	else
	{
		PlatformFile.FindFiles(FoundFiles, *Path, *FileExtensionIncludeDot);
	}
}

bool UMESFileUtility::CreateSpecificFile(FString Path, FString FileName, FString FileExtensionIncludeDot)
{
	if (!Path.EndsWith("\\") || !Path.EndsWith("/"))
	{
		Path.Append("\\");
	}
	FString FileFullPath = Path + FileName + FileExtensionIncludeDot;
	return CreateFileWithFullPath(FileFullPath);
}

bool UMESFileUtility::CreateFileWithFullPath(FString FileFullPath)
{
	return FFileHelper::SaveStringToFile(TEXT(""), *FileFullPath);
}

bool UMESFileUtility::OpenConfirmDialog(FString Message)
{
	HWND hWnd_ue = (HWND)GEngine->GameViewport->GetWindow()->GetNativeWindow()->GetOSWindowHandle();
	FString Title("Confirm");
	int ConfirmResult = MessageBox(hWnd_ue, (LPCWSTR)*Message, (LPCWSTR)*Title, MB_OKCANCEL | MB_ICONQUESTION | MB_APPLMODAL);
	if (ConfirmResult == 0 || ConfirmResult == IDCANCEL)
	{
		return false;
	}
	else if(ConfirmResult == IDOK)
	{
		return true;
	}
	return false;
}

bool UMESFileUtility::CopyFileFromTo(FString FromPath, FString ToPath)
{
#ifdef CopyFile
#undef CopyFile
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	bool Result = PlatformFile.CopyFile(*ToPath, *FromPath);
#endif // CopyFile
	return Result;
}

bool UMESFileUtility::CreateDirectoryAt(FString DirectoryPath)
{
#ifdef CreateDirectory
#undef CreateDirectory
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	bool Result = PlatformFile.CreateDirectoryTree(*DirectoryPath);
#endif // CreateDirectory
	return Result;
}

bool UMESFileUtility::DeleteDic(FString DicPath)
{
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	return PlatformFile.DeleteDirectoryRecursively(*DicPath);
}

bool UMESFileUtility::SaveStringToPath(FString Content, FString FilePath, FString FileName)
{
	FString  File = FilePath + "//" + FileName;

	return  FFileHelper::SaveStringToFile(Content, *File);
}

FString UMESFileUtility::ReadFile(FString FilePath, FString FileName)
{
	FString  TEMP = FilePath + "//" + FileName;
	FString Content;
	Content.Empty();
	FFileHelper::LoadFileToString(Content, *TEMP, FFileHelper::EHashOptions::None, 0);
	if (Content.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Not found file!"));
	}
	return Content;
}


