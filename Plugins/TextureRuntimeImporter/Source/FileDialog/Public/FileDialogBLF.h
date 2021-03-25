// Copyright 2020 byTangs, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FileDialogBLF.generated.h"

/**
 * 
 */

/** Details about the dialog */
UENUM(BlueprintType)
enum class EFileDialogFlags :uint8
{
	Single      = 0,
	Multiple
};





UCLASS()
class FILEDIALOG_API UFileDialogBLF : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()


public:

	/** 
	* Opens the "open file" dialog for the platform
	* @param DialogTitle				The text for the title of the dialog window
	* @param DefaultPath				The path where the file dialog will open initially
	* @param DefaultFile				The file that the dialog will select initially
	* @param Flags						Details about the dialog. Single Or Multiple
	* @param FileTypes					The type filters to show in the dialog. This string should be a "|" delimited list of (Description|Extensionlist) pairs. Extensionlists are ";" delimited.
	* @param OutFilenames				The filenames that were selected in the dialog
	* @return true if files were successfully selected
	*/
	UFUNCTION(BlueprintCallable,Category= "FileDialog")
	static bool OpenFileDialog(const FString& DialogTitle, const FString& DefaultPath, const FString& DefaultFile, const FString& FileTypes, EFileDialogFlags Flags, TArray<FString>& OutFilenames);

	/** 
	* Opens the "save file" dialog for the platform
	* @param DialogTitle				The text for the title of the dialog window
	* @param DefaultPath				The path where the file dialog will open initially
	* @param DefaultFile				The file that the dialog will select initially
	* @param Flags						Details about the dialog. See EFileDialogFlags.
	* @param FileTypes					The type filters to show in the dialog. This string should be a "|" delimited list of (Description|Extensionlist) pairs. Extensionlists are ";" delimited.
	* @param OutFilenames				The filenames that were selected in the dialog
	* @return true if files were successfully selected
	*/
	UFUNCTION(BlueprintCallable,Category= "FileDialog")
	static bool SaveFileDialog(const FString& DialogTitle, const FString& DefaultPath, const FString& DefaultFile, const FString& FileTypes, EFileDialogFlags Flags, TArray<FString>& OutFilenames);


	/** 
	* Opens the "choose folder" dialog for the platform
	*
	* @param DialogTitle				The text for the title of the dialog window
	* @param DefaultPath				The path where the file dialog will open initially
	* @param OutFolderName				The FolderName that was selected in the dialog
	* @return true if folder choice was successfully selected
	*/
	UFUNCTION(BlueprintCallable,Category= "FileDialog")
	static bool OpenDirectoryDialog(const FString& DialogTitle, const FString& DefaultPath, FString& OutFolderName);
};
