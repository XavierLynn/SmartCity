// Copyright 2020 byTangs, All Rights Reserved.

#include "FileDialogBLF.h"
#include "FileDialog.h"
#include "Framework/Application/SlateApplication.h"

bool UFileDialogBLF::OpenFileDialog(const FString& DialogTitle, const FString& DefaultPath, const FString& DefaultFile, const FString& FileTypes, EFileDialogFlags Flags,
                                    TArray<FString>& OutFilenames)
{
    const void* WindowHandle = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr);
    return FFileDialog::Get()->OpenFileDialog(WindowHandle,DialogTitle,DefaultPath,DefaultFile,FileTypes,static_cast<uint32>(Flags),OutFilenames);
}

bool UFileDialogBLF::SaveFileDialog(const FString& DialogTitle, const FString& DefaultPath, const FString& DefaultFile, const FString& FileTypes, EFileDialogFlags Flags,
    TArray<FString>& OutFilenames)
{
    const void* WindowHandle = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr);
    return  FFileDialog::Get()->SaveFileDialog(WindowHandle,DialogTitle,DefaultPath,DefaultFile,FileTypes,static_cast<uint32>(Flags),OutFilenames);
}

bool UFileDialogBLF::OpenDirectoryDialog(const FString& DialogTitle, const FString& DefaultPath, FString& OutFolderName)
{
    const void* WindowHandle = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr);
    return  FFileDialog::Get()->OpenDirectoryDialog(WindowHandle,DialogTitle,DefaultPath,OutFolderName);
}
