// Copyright 2020 byTangs, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"


class IFileDialogBase
{
public:

    /** Destructor */
    virtual ~IFileDialogBase() { };

    /** 
    * Opens the "open file" dialog for the platform
    *
    * @param ParentWindowHandle		    The native handle to the parent window for this dialog
    * @param DialogTitle				The text for the title of the dialog window
    * @param DefaultPath				The path where the file dialog will open initially
    * @param DefaultFile				The file that the dialog will select initially
    * @param Flags						Details about the dialog. See EFileDialogFlags.
    * @param FileTypes					The type filters to show in the dialog. This string should be a "|" delimited list of (Description|Extensionlist) pairs. Extensionlists are ";" delimited.
    * @param OutFilenames				The filenames that were selected in the dialog
    * @return true if files were successfully selected
    */
    virtual bool OpenFileDialog(const void* ParentWindowHandle, const FString& DialogTitle, const FString& DefaultPath, const FString& DefaultFile, const FString& FileTypes, uint32 Flags, TArray<FString>& OutFilenames) = 0;

    /** 
    * Opens the "save file" dialog for the platform
    *
    * @param ParentWindowHandle		The native handle to the parent window for this dialog
    * @param DialogTitle				The text for the title of the dialog window
    * @param DefaultPath				The path where the file dialog will open initially
    * @param DefaultFile				The file that the dialog will select initially
    * @param Flags						Details about the dialog. See EFileDialogFlags.
    * @param FileTypes					The type filters to show in the dialog. This string should be a "|" delimited list of (Description|Extensionlist) pairs. Extensionlists are ";" delimited.
    * @param OutFilenames				The filenames that were selected in the dialog
    * @return true if files were successfully selected
    */
    virtual bool SaveFileDialog(const void* ParentWindowHandle, const FString& DialogTitle, const FString& DefaultPath, const FString& DefaultFile, const FString& FileTypes, uint32 Flags, TArray<FString>& OutFilenames) = 0;

    /** 
    * Opens the "choose folder" dialog for the platform
    *
    * @param ParentWindowHandle		The native handle to the parent window for this dialog
    * @param DialogTitle				The text for the title of the dialog window
    * @param DefaultPath				The path where the file dialog will open initially
    * @param OutFolderName				The FolderName that was selected in the dialog
    * @return true if folder choice was successfully selected
    */
    virtual bool OpenDirectoryDialog(const void* ParentWindowHandle, const FString& DialogTitle, const FString& DefaultPath, FString& OutFolderName) = 0;
};