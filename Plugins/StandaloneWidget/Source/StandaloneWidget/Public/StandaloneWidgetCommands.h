// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "StandaloneWidgetStyle.h"

class FStandaloneWidgetCommands : public TCommands<FStandaloneWidgetCommands>
{
public:

	FStandaloneWidgetCommands()
		: TCommands<FStandaloneWidgetCommands>(TEXT("StandaloneWidget"), NSLOCTEXT("Contexts", "StandaloneWidget", "StandaloneWidget Plugin"), NAME_None, FStandaloneWidgetStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};