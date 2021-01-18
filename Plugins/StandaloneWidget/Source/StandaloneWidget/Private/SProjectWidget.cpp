// Fill out your copyright notice in the Description page of Project Settings.


#include "SProjectWidget.h"
#include "SlateOptMacros.h"

#include "Widgets/Layout/SExpandableArea.h"
#include "SHotPatcherPlatformListRow.h"

#define LOCTEXT_NAMESPACE "SProjectCookPage"


//BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SProjectWidget::Construct(const FArguments& InArgs)
{
	MakePlatformMenu();
	ChildSlot
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
					.FillWidth(1.0)
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("WhichProjectToUseText", "How would you like to cook the content?"))
					]
				+ SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(8.0, 0.0, 0.0, 0.0)
					[
						SNew(SButton)
						.Text(LOCTEXT("ImportConfig", "Import"))
						//.OnClicked(this, &SProjectCookPage::DoImportConfig)
					]
				+ SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(5.0, 0.0, 0.0, 0.0)
					[
						SNew(SButton)
						.Text(LOCTEXT("ExportConfig", "Export"))
						//.OnClicked(this, &SProjectCookPage::DoExportConfig)
					]
				+ SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(5.0, 0.0, 0.0, 0.0)
					[
						SNew(SButton)
						.Text(LOCTEXT("ResetConfig", "Reset"))
						//.OnClicked(this, &SProjectCookPage::DoResetConfig)
					]
			]
			+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.0, 8.0, 0.0, 0.0)
				[
					SNew(SExpandableArea)
					.AreaTitle(LOCTEXT("CookPlatforms", "Platforms"))
					.InitiallyCollapsed(true)
					.Padding(8.0)
					.BodyContent()
					[
						SNew(SVerticalBox)
						+ SVerticalBox::Slot()
							.AutoHeight()
							[
								SNew(SVerticalBox)
								+ SVerticalBox::Slot()
									.AutoHeight()
									.MaxHeight(256.0f)
									[
										// platform menu
										SAssignNew(PlatformListView, SListView<TSharedPtr<FString> >)
										.HeaderRow(
											SNew(SHeaderRow)
											.Visibility(EVisibility::Collapsed)
											+ SHeaderRow::Column("PlatformName")
											.DefaultLabel(LOCTEXT("PlatformListPlatformNameColumnHeader", "Platform"))
											.FillWidth(1.0f)
										)
										.ItemHeight(16.0f)
										.ListItemsSource(&PlatformList)
										.OnGenerateRow(this, &SProjectWidget::HandlePlatformListViewGenerateRow)
										.SelectionMode(ESelectionMode::None)
										]
									]
								]

						]
		];
		

	
}
//
//
TSharedRef<ITableRow> SProjectWidget::HandlePlatformListViewGenerateRow(TSharedPtr<FString> InItem, const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(STableRow<TSharedPtr<FString>>, OwnerTable)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			[
				SNew(SCheckBox)
			]
	+ SHorizontalBox::Slot()
		[
			SNew(STextBlock)
			.Text(FText::FromString(*InItem))
		]
	
		];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
#undef LOCTEXT_NAMESPACE