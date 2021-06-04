// Fill out your copyright notice in the Description page of Project Settings.


#include "MESPlayerController.h"
#include "Engine.h"
#include "MESProduct/MESComponent/Http/HttpNetComponent.h"
#include "MESProduct/MESComponent/RabbitMq/Public/RabbitMqClient.h"
#include "Engine/UserInterfaceSettings.h"

TMap<FString, FMESStringFilter> AMESPlayerController::StringFilterMap;
MESStringFilter::FItemToStringArray AMESPlayerController::ItemToStringDelegate;


AMESPlayerController::AMESPlayerController(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	HttpComponent = CreateDefaultSubobject<UHttpNetComponent>(TEXT("HttpComponent"));
	RabbitMqComponent = CreateDefaultSubobject<URabbitMqClient>(TEXT("RabbitMqComponent"));
}

void AMESPlayerController::SetDPIScaleFactor(float Value)
{
	//GEngine->GameViewport->GetWindow()->GetSizeLimits().SetMinWidth(1280).SetMinHeight(720);
	//GEngine->GameViewport->GetWindow()->GetNativeWindow()->SetDPIScaleFactor(Value);

	GetMutableDefault<UUserInterfaceSettings>()->ApplicationScale = Value;
}

void AMESPlayerController::SetStringFilter(FString FilterName, FString FilterString, FSetFilterDelegate Callback)
{
	if (StringFilterMap.Contains(FilterName))
	{
		if (StringFilterMap[FilterName].Filter.IsValid())
		{
			StringFilterMap[FilterName].Filter->OnChanged().AddLambda([&]() {
				if (Callback.IsBound())
				{
					Callback.Execute();
					Callback.Unbind();
				}
			});
			StringFilterMap[FilterName].Filter->SetRawFilterText(FText::FromString(FilterString));
		}
	}
	else
	{
		FMESStringFilter NewFilter;
		NewFilter.FilterName = FilterName;
		NewFilter.Filter = MakeShareable(new MESStringFilter(ItemToStringDelegate));
		if (NewFilter.Filter.IsValid())
		{
			NewFilter.Filter->OnChanged().AddLambda([&]() {
				if (Callback.IsBound())
				{
					Callback.Execute();
					Callback.Unbind();
				}
			});
		}

		StringFilterMap.Add(FilterName, NewFilter);

		if (NewFilter.Filter.IsValid())
		{
			NewFilter.Filter->SetRawFilterText(FText::FromString(FilterString));
		}
	}
}

bool AMESPlayerController::TestFilter(FString FilterName, FString TestString)
{
	if (StringFilterMap.Contains(FilterName) && StringFilterMap[FilterName].Filter.IsValid())
	{
		return StringFilterMap[FilterName].Filter->PassesFilter(TestString);
	}
	return true;
}

void AMESPlayerController::BeginPlay()
{
	Super::BeginPlay();
	ItemToStringDelegate.BindUObject(this, &AMESPlayerController::ItemToString);
}

void AMESPlayerController::ItemToString(const FString & SourceData, TArray<FString>& OutSearchStrings)
{
	OutSearchStrings.Add(SourceData);
}
