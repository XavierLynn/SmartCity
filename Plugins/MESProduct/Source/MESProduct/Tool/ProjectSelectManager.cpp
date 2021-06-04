// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectSelectManager.h"
#include "ConfigManager.h"
#include "MESProduct/UI/Launcher/LauncherDataAsset.h"
#include "Components/TextBlock.h"
#include "UnrealClient.h"
#include "Blueprint/WidgetTree.h"
#include "Engine.h"
//#include "Kismet/GameplayStatics.h"
//#include "PlayerController.h"

void UProjectSelectManager::NativeConstruct()
{
	Super::NativeConstruct();

	ULauncherDataAsset* Config = UConfigManager::Get()->LauncherConfig;
	if (!Config || Config->Version.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("can not find version config"));
		return;
	}

	UTextBlock* VersionText = WidgetTree->FindWidget<UTextBlock>("TextBlock_Version");
	if (VersionText && Config && !Config->Version.IsEmpty())
	{
		VersionText->SetText(FText::FromString(Config->Version));
	}
	
	//GEngine->GameViewport->Viewport->ViewportResizedEvent.AddUObject(this, &UProjectSelectManager::OnViewportResized);
	
}


void UProjectSelectManager::OnViewportResized(FViewport* Viewport, uint32 Value)
{
	//UE_LOG(LogTemp, Warning, TEXT("OnViewportResized, Value: %f"), Value);

	int SizeX = Viewport->GetSizeXY().X;
	int SizeY = Viewport->GetSizeXY().Y;
	if (OnMESViewportResized.IsBound() && Viewport)
	{
		OnMESViewportResized.Broadcast(SizeX, SizeY);
	}
	/*GEngine->GameViewport->GetWindow()->GetSizeLimits().SetMinWidth(1280).SetMinHeight(720);
	FVector2D NewSize(SizeX, SizeY);
	GEngine->GameViewport->GetWindow()->Resize(NewSize);*/
	if (SizeX < 1280 || SizeY < 720)
	{
		int NewSizeX = SizeX < 1280 ? 1280 : SizeX;
		int NewSizeY = SizeY < 720 ? 720 : SizeY;
		FVector2D NewSize(NewSizeX, NewSizeY);
		GEngine->GameViewport->GetWindow()->Resize(NewSize);
	}
}

void UProjectSelectManager::NativeTick(const FGeometry & MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

bool UProjectSelectManager::CheckUrl(FString InUrl)
{
	bool HttpCheck = InUrl.StartsWith("http://");
	bool LengthCheck = InUrl.Len() > 7;
	return HttpCheck && LengthCheck;
}