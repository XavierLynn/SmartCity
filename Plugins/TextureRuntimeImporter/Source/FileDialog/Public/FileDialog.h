// Copyright 2020 byTangs, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "IFileDialogBase.h"
#include "Modules/ModuleManager.h"


// platform file dialog  
class FFileDialog : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	void StartupModule() override;
	void ShutdownModule() override;


public:

	static IFileDialogBase* Get()
	{
		FFileDialog FileDialogModule = FModuleManager::Get().LoadModuleChecked<FFileDialog>("FileDialog");
		return FileDialogModule.GetSingleton();
	}

	
private:

	/** FileDialog Singleton */
	IFileDialogBase* GetSingleton()const
	{
		return FileDialog;
	}

	
	/** File dialog */
	IFileDialogBase*  FileDialog;
};
