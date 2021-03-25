// Copyright 2020 byTangs, All Rights Reserved.

#include "FileDialog.h"
#include "Modules/ModuleManager.h"
#include "FileDialogPrivate.h"

#define LOCTEXT_NAMESPACE "FFileDialog"

void FFileDialog::StartupModule()
{
	FileDialog = new PlatformFileDialog();
}

void FFileDialog::ShutdownModule()
{
	if (FileDialog != nullptr)
	{
		delete FileDialog;
		FileDialog = nullptr;
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FFileDialog, FileDialog);