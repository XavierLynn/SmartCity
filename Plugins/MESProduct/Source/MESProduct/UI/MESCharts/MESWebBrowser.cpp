// Fill out your copyright notice in the Description page of Project Settings.


#include "MESWebBrowser.h"
#include "WebBrowser/Public/SWebBrowser.h"

void UMESWebBrowser::BindToObject(const FString& Name, UObject* Object, bool bIsPermanent /*= true*/)
{
	if (WebBrowserWidget.IsValid())
	{
		WebBrowserWidget->BindUObject(Name, Object, bIsPermanent);
	}
}

bool UMESWebBrowser::IsLoaded()
{
	if (WebBrowserWidget.IsValid())
	{
		return WebBrowserWidget->IsLoaded();
	}
	return false;
}

void UMESWebBrowser::ReleaseSlateResources(bool bReleaseChildren)
{
	if (CanConstruct)
	{
		Super::ReleaseSlateResources(bReleaseChildren);
	}
}

TSharedRef<SWidget> UMESWebBrowser::RebuildWidget()
{
	if (CanConstruct)
	{
		return Super::RebuildWidget();
	}
	return WebBrowserWidget.ToSharedRef();
}
