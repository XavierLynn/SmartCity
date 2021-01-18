// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

#include "SlateBasics.h"





class STANDALONEWIDGET_API SProjectWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SProjectWidget)
	{}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
public:

	TSharedRef<ITableRow> HandlePlatformListViewGenerateRow(TSharedPtr<FString> InItem, const TSharedRef<STableViewBase>& OwnerTable);

	
	void MakePlatformMenu()
	{
		//PlatformList.Reset();
		//PlatformList.Add(MakeShareable(new FString("Hello 1")));
		
		TArray<ITargetPlatform*> Platforms = GetTargetPlatformManager()->GetTargetPlatforms();

		if (Platforms.Num() > 0)
		{
			PlatformList.Reset();
			for (int32 PlatformIndex = 0; PlatformIndex < Platforms.Num(); ++PlatformIndex)
			{
				FString PlatformName = Platforms[PlatformIndex]->PlatformName();

				PlatformList.Add(MakeShareable(new FString(PlatformName)));
			}
		}
	}
private:
	TArray<TSharedPtr<FString> > PlatformList;
	//TSharedPtr<FHotPatcherCookModel> CookModel;
	TSharedPtr<SListView<TSharedPtr<FString> > > PlatformListView;
	
	//TSharedPtr<FHotPatcherCookModel> mCookModel;
};
