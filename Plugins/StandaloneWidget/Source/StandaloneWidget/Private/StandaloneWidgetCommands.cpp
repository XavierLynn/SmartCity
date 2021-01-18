// Copyright Epic Games, Inc. All Rights Reserved.

#include "StandaloneWidgetCommands.h"

#define LOCTEXT_NAMESPACE "FStandaloneWidgetModule"

void FStandaloneWidgetCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "StandaloneWidget", "Bring up StandaloneWidget window", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
