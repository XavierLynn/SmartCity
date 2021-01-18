// Copyright Epic Games, Inc. All Rights Reserved.

#include "StandaloneWidget.h"
#include "StandaloneWidgetStyle.h"
#include "StandaloneWidgetCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SGridPanel.h"
#include "SProjectWidget.h"


static const FName StandaloneWidgetTabName("StandaloneWidget");

#define LOCTEXT_NAMESPACE "FStandaloneWidgetModule"

void FStandaloneWidgetModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FStandaloneWidgetStyle::Initialize();
	FStandaloneWidgetStyle::ReloadTextures();

	FStandaloneWidgetCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FStandaloneWidgetCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FStandaloneWidgetModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FStandaloneWidgetModule::RegisterMenus));
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(StandaloneWidgetTabName, FOnSpawnTab::CreateRaw(this, &FStandaloneWidgetModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FStandaloneWidgetTabTitle", "StandaloneWidget"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FStandaloneWidgetModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FStandaloneWidgetStyle::Shutdown();

	FStandaloneWidgetCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(StandaloneWidgetTabName);
}

TSharedRef<SDockTab> FStandaloneWidgetModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FText WidgetText = FText::Format(
		LOCTEXT("WindowWidgetText", "Add code to {0} in {1} to override this window's contents"),
		FText::FromString(TEXT("FStandaloneWidgetModule::OnSpawnPluginTab")),
		FText::FromString(TEXT("StandaloneWidget.cpp"))
		);
	return SAssignNew(DockTab, SDockTab)
		.TabRole(ETabRole::NomadTab)
		.Label(LOCTEXT("HotPatcherTab", "Hot Patcher"))
		.ToolTipText(LOCTEXT("HotPatcherTabTextToolTip", "Hot Patcher"))
		.Clipping(EWidgetClipping::ClipToBounds)
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			.HAlign(HAlign_Fill)
			[
				SNew(SScrollBox)
				+ SScrollBox::Slot()
					.Padding(0.0f, 10.0f, 8.0f, 0.0f)
					[
						SNew(SGridPanel)
						.FillColumn(1, 1.0f)

						// cook section
						+ SGridPanel::Slot(0, 0)
							.Padding(8.0f, 10.0f, 0.0f, 0.0f)
							.VAlign(VAlign_Top)
							[
								SNew(STextBlock)
								.Font(FCoreStyle::GetDefaultFontStyle("Cook", 13))
								.Text(LOCTEXT("ProjectCookSectionHeader", "Cook"))
							]

						+ SGridPanel::Slot(1, 0)
							.Padding(32.0f, 0.0f, 8.0f, 0.0f)
							[
								SNew(SProjectWidget)
							]
					]
			]
			];
}
	 
	
	


void FStandaloneWidgetModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->InvokeTab(StandaloneWidgetTabName);
}

void FStandaloneWidgetModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FStandaloneWidgetCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FStandaloneWidgetCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FStandaloneWidgetModule, StandaloneWidget)