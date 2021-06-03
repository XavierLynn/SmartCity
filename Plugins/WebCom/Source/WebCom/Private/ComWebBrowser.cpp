// Copyright 2021 x.Lin. All Rights Reserved

#include "ComWebBrowser.h"
#include "WebCom/EMWebInterface/WebInterfaceObject.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Async/TaskGraphInterfaces.h"
#include "UObject/ConstructorHelpers.h"
#include "Runtime/Slate/Public/Framework/Application/SlateApplication.h"

#if WITH_EDITOR
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialExpressionMaterialFunctionCall.h"
#include "Materials/MaterialExpressionTextureSample.h"
#include "Materials/MaterialExpressionTextureSampleParameter2D.h"
#include "Materials/MaterialFunction.h"
#include "Factories/MaterialFactoryNew.h"
#include "AssetRegistryModule.h"
#include "PackageHelperFunctions.h"
#endif

#define LOCTEXT_NAMESPACE "UComWebBrowser"


UComWebBrowser::UComWebBrowser(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsVariable = true;
}


void UComWebBrowser::LoadURL(FString NewURL)
{
	if (ComWebBrowser.IsValid())
	{
		return ComWebBrowser->LoadURL(NewURL);
	}
}

void UComWebBrowser::LoadString(FString Contents, FString DummyURL)
{
	if (ComWebBrowser.IsValid())
	{
		return ComWebBrowser->LoadString(Contents, DummyURL);
	}
}

void UComWebBrowser::ExecuteJavascript(const FString& ScriptText)
{
	if (ComWebBrowser.IsValid())
	{
		return ComWebBrowser->ExecuteJavascript(ScriptText);
	}
}

FText UComWebBrowser::GetTitleText() const
{
	if (ComWebBrowser.IsValid())
	{
		return ComWebBrowser->GetTitleText();
	}

	return FText::GetEmpty();
}

FString UComWebBrowser::GetUrl() const
{
	if (ComWebBrowser.IsValid())
	{
		return ComWebBrowser->GetUrl();
	}

	return FString();
}

void UComWebBrowser::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	ComWebBrowser.Reset();
}

void UComWebBrowser::Call(const FString& Function, const FJsonLibraryValue& Data)
{

	if (!ComWebBrowser.IsValid())
		return;
	FString CurString = FString::Printf(TEXT("ue.interface[%s](%s)"),
		*FJsonLibraryValue(Function).Stringify(),
		*Data.Stringify());
	if (Data.GetType() != EJsonLibraryType::Invalid)
		ComWebBrowser->ExecuteJavascript(FString::Printf(TEXT("ue.interface[%s](%s)"),
			*FJsonLibraryValue(Function).Stringify(),
			*Data.Stringify()));
	else
		ComWebBrowser->ExecuteJavascript(FString::Printf(TEXT("ue.interface[%s]()"),
			*FJsonLibraryValue(Function).Stringify()));

}

TSharedRef<SWidget> UComWebBrowser::RebuildWidget()
{
	if (IsDesignTime())
	{
		return SNew(SBox)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("UComWebBrowser", "UComWebBrowser"))
			];
	}
	else
	{
		ComWebBrowser = SNew(SWebBrowser)
			.InitialURL(InitialURL)
			/*.UseChartDPI(false)*/
			.SupportsTransparency(bSupportsTransparency)
			.OnUrlChanged(BIND_UOBJECT_DELEGATE(FOnTextChanged, HandleOnUrlChanged))
			.OnBeforePopup(BIND_UOBJECT_DELEGATE(FOnBeforePopupDelegate, HandleOnBeforePopup));

		MyObject = NewObject<UWebInterfaceObject>();
		if (MyObject)
		{
			MyObject->MyInterface = this;
			ComWebBrowser->BindUObject("interface", MyObject);
		}
		return ComWebBrowser.ToSharedRef();
	}

}

void UComWebBrowser::Bind(const FString& Name, UObject* Object)
{
	if (!Object)
		return;
	if (ComWebBrowser.IsValid())
		ComWebBrowser->BindUObject(Name, Object);
}

void UComWebBrowser::Unbind(const FString& Name, UObject* Object)
{
	if (!Object)
		return;
	if (ComWebBrowser.IsValid())
		ComWebBrowser->UnbindUObject(Name, Object);
}

void UComWebBrowser::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (ComWebBrowser.IsValid())
	{

	}
}

void UComWebBrowser::HandleOnUrlChanged(const FText& InText)
{
	OnUrlChanged.Broadcast(InText);
}

bool UComWebBrowser::HandleOnBeforePopup(FString URL, FString Frame)
{
	if (OnBeforePopup.IsBound())
	{
		if (IsInGameThread())
		{
			OnBeforePopup.Broadcast(URL, Frame);
		}
		else
		{
			// Retry on the GameThread.
			TWeakObjectPtr<UComWebBrowser> WeakThis = this;
			FFunctionGraphTask::CreateAndDispatchWhenReady([WeakThis, URL, Frame]()
				{
					if (WeakThis.IsValid())
					{
						WeakThis->HandleOnBeforePopup(URL, Frame);
					}
				}, TStatId(), nullptr, ENamedThreads::GameThread);
		}

		return true;
	}

	return false;
}

#if WITH_EDITOR

const FText UComWebBrowser::GetPaletteCategory()
{
	return LOCTEXT("ComWebBrowser", "ComWebBrowser");
}

#endif

/////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
