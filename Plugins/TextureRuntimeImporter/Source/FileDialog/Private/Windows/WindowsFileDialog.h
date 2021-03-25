// Copyright 2020 byTangs, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IFileDialogBase.h"


class FWindowsFileDialog : public IFileDialogBase
{
public:

    /** override base OpenFileDialog function */
    virtual bool OpenFileDialog(const void* ParentWindowHandle, const FString& DialogTitle, const FString& DefaultPath, const FString& DefaultFile, const FString& FileTypes, uint32 Flags, TArray<FString>& OutFilenames) override;

    /** override base SaveFileDialog function */
    virtual bool SaveFileDialog(const void* ParentWindowHandle, const FString& DialogTitle, const FString& DefaultPath, const FString& DefaultFile, const FString& FileTypes, uint32 Flags, TArray<FString>& OutFilenames) override;

    /** override base OpenDirectoryDialog function */
    virtual bool OpenDirectoryDialog(const void* ParentWindowHandle, const FString& DialogTitle, const FString& DefaultPath, FString& OutFolderName) override;

private:
    
    /** Generic function for windows platform file dialog */
    bool FileDialogShared(bool bSave, const void* ParentWindowHandle, const FString& DialogTitle, const FString& DefaultPath, const FString& DefaultFile, const FString& FileTypes, uint32 Flags, TArray<FString>& OutFilenames, int32& OutFilterIndex);
};

typedef FWindowsFileDialog PlatformFileDialog;