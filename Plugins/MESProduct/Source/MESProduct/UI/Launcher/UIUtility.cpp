// Fill out your copyright notice in the Description page of Project Settings.


#include "UIUtility.h"
#include "Blueprint/UserWidget.h"
#include "EngineUtils.h"
#include "Engine.h"
#include "HighResScreenshot.h"
#include "ImageUtils.h"
#include "Slate/SceneViewport.h"
#include "MESProduct/Tool/MESPathUtility.h"
#include "IDesktopPlatform.h"

bool UUIUtility::ScreenShotWithUI(FVector2D  MinPos, FVector2D MaxPos)
{

	/*FString FileName = UMESPathUtility::GetScreenShotPath() + FDateTime::Now().ToString() + TEXT(".png");
	FScreenshotRequest::RequestScreenshot(FileName, true, true);
	FPlatformProcess::ExploreFolder(*UMESPathUtility::GetScreenShotPath());

	return;*/
	FIntRect Rect(MinPos.X,MinPos.Y,MaxPos.X,MaxPos.Y);
	FIntVector Size(FMath::Abs((MaxPos - MinPos).X), FMath::Abs((MaxPos - MinPos).Y), 0);
	FString ScreenShotName = UMESPathUtility::GetScreenShotPath() + FDateTime::Now().ToString() + TEXT(".png");
	TArray<FColor> Bitmap; 
	GScreenshotResolutionX = Size.X;
	GScreenshotResolutionY = Size.Y;
	UGameViewportClient* ViewportClient = GEngine->GameViewport;
	TSharedRef<SWidget> WindowRef = ViewportClient->GetWindow().ToSharedRef();
	TSharedPtr<SWindow> WidgetWindow = FSlateApplication::Get().FindWidgetWindow(WindowRef);
	if (!WidgetWindow.IsValid())
	{
		return false;
	}

	TSharedRef<SWindow> CurrentWindowRef = WidgetWindow.ToSharedRef();

	FWidgetPath WidgetPath;
	FSlateApplication::Get().GeneratePathToWidgetChecked(WindowRef, WidgetPath);

	FArrangedWidget ArrangedWidget = WidgetPath.FindArrangedWidget(WindowRef).Get(FArrangedWidget::GetNullWidget());
	FVector2D Position = ArrangedWidget.Geometry.AbsolutePosition;
	//FVector2D Size = ArrangedWidget.Geometry.GetDrawSize();
	FVector2D WindowPosition = WidgetWindow->GetPositionInScreen();

	FIntRect ScreenshotRect = Rect;

	ScreenshotRect.Min.X += (Position.X - WindowPosition.X);
	ScreenshotRect.Min.Y += (Position.Y - WindowPosition.Y);
	ScreenshotRect.Max.X += (Position.X - WindowPosition.X);
	ScreenshotRect.Max.Y += (Position.Y - WindowPosition.Y);
#if ENGINE_MINOR_VERSION >= 26
	FSlateApplication::Get().GetRenderer()->PrepareToTakeScreenshot(ScreenshotRect, &Bitmap,&(*WidgetWindow));
#else
	FSlateApplication::Get().GetRenderer()->PrepareToTakeScreenshot(ScreenshotRect, &Bitmap);
#endif
	FSlateApplication::Get().ForceRedrawWindow(WidgetWindow.ToSharedRef());


	if (GIsDumpingMovie && ScreenShotName.IsEmpty())
	{
		// Request a new screenshot with a formatted name
		const bool bAddFilenameSuffix = true;
		FScreenshotRequest::RequestScreenshot(FString(), true, bAddFilenameSuffix);
		ScreenShotName = FScreenshotRequest::GetFilename();
	}


	GetHighResScreenshotConfig().MergeMaskIntoAlpha(Bitmap);

	FIntRect SourceRect(0, 0, GScreenshotResolutionX, GScreenshotResolutionY);

	// Clip the bitmap to just the capture region if valid
	if (!SourceRect.IsEmpty())
	{
		FColor* const Data = Bitmap.GetData();
		const int32 OldWidth = Size.X;
		const int32 OldHeight = Size.Y;
		const int32 NewWidth = SourceRect.Width();
		const int32 NewHeight = SourceRect.Height();
		const int32 CaptureTopRow = SourceRect.Min.Y;
		const int32 CaptureLeftColumn = SourceRect.Min.X;

		for (int32 Row = 0; Row < NewHeight; Row++)
		{
			FMemory::Memmove(Data + Row * NewWidth, Data + (Row + CaptureTopRow) * OldWidth + CaptureLeftColumn, NewWidth * sizeof(*Data));
		}

		Bitmap.RemoveAt(NewWidth * NewHeight, OldWidth * OldHeight - NewWidth * NewHeight, false);
		Size = FIntVector(NewWidth, NewHeight, 0);
	}

	if (!FPaths::GetExtension(ScreenShotName).IsEmpty())
	{
		ScreenShotName = FPaths::GetBaseFilename(ScreenShotName, false);
		ScreenShotName += TEXT(".png");
	}

	// Save the contents of the array to a png file.
	TArray<uint8> CompressedBitmap;
	FImageUtils::CompressImageArray(Size.X, Size.Y, Bitmap, CompressedBitmap);
	bool bIsScreenshotSaved = FFileHelper::SaveArrayToFile(CompressedBitmap, *ScreenShotName);
	if (bIsScreenshotSaved)
	{
		FPlatformProcess::ExploreFolder(*UMESPathUtility::GetScreenShotPath());
	}
	FScreenshotRequest::Reset();
	FScreenshotRequest::OnScreenshotRequestProcessed().Broadcast();
	GAreScreenMessagesEnabled = GScreenMessagesRestoreState;
	return bIsScreenshotSaved;
}

bool UUIUtility::HighResScreenshotByRect(FVector2D  MinPos, FVector2D MaxPos)
{
	FString FileName = UMESPathUtility::GetScreenShotPath() + FDateTime::Now().ToString() + TEXT(".png");
	//GetHighResScreenshotConfig().SetResolution(1920, 1080);
	GScreenshotResolutionX = 0;
	GScreenshotResolutionY = 0;
	GetHighResScreenshotConfig().CaptureRegion = FIntRect(MinPos.X,MinPos.Y,MaxPos.X,MaxPos.Y);
	GetHighResScreenshotConfig().SetFilename(FileName);
	bool bIsShotSuccess = GEngine->GameViewport->GetGameViewport()->TakeHighResScreenShot();
	if (bIsShotSuccess)
	{
		FPlatformProcess::ExploreFolder(*UMESPathUtility::GetScreenShotPath());
	}
	FScreenshotRequest::Reset();
	FScreenshotRequest::OnScreenshotRequestProcessed().Broadcast();
	return bIsShotSuccess;
}

bool UUIUtility::HighResScreenshotByReslution(float Width, float Height)
{
	FString FileName = UMESPathUtility::GetScreenShotPath() + FDateTime::Now().ToString() + TEXT(".png");
	GScreenshotResolutionX = 0;
	GScreenshotResolutionY = 0;
	GetHighResScreenshotConfig().SetResolution(Width, Height);
	GetHighResScreenshotConfig().SetFilename(FileName);
	bool bIsShotSuccess = GEngine->GameViewport->GetGameViewport()->TakeHighResScreenShot();
	if (bIsShotSuccess)
	{
		FPlatformProcess::ExploreFolder(*UMESPathUtility::GetScreenShotPath());
	}
	FScreenshotRequest::Reset();
	FScreenshotRequest::OnScreenshotRequestProcessed().Broadcast();
	return bIsShotSuccess;
}
