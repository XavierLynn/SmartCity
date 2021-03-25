/************************************************************************/
/* Author: YWT20                                                        */
/* Expected release year : 2020                                         */
/************************************************************************/

#include "WidgetWebCamera.h"

UWidgetWebCamera::UWidgetWebCamera(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsAutoPlay = false;
	CameraURL = "";
	RefreshRate = 15;
	isStreamOpen = false;
	VideoSize = FVector2D(0, 0);
	ShouldResize = false;
	ResizeDeminsions = FVector2D(320, 240);
	RefreshTimer = 0.0f;
}

void UWidgetWebCamera::NativeConstruct()
{
	Super::NativeConstruct();
	stream = cv::VideoCapture();
	frame = cv::Mat();
	if (bIsAutoPlay && !CameraURL.IsEmpty())
	{
		OpenWebCamera(CameraURL);
	}
}

void UWidgetWebCamera::NativeDestruct()
{
	Super::NativeDestruct();
	stream.release();
}

void UWidgetWebCamera::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	GInitRunaway();

	TickActionsAndAnimation(MyGeometry, InDeltaTime);

	RefreshTimer += InDeltaTime;
	if (isStreamOpen && RefreshTimer >= 1.0f / RefreshRate)
	{
		RefreshTimer -= 1.0f / RefreshRate;
		UpdateFrame();
		UpdateTexture();
		OnUpdateWebCameraTexture(VideoTexture);
	}

	Tick(MyGeometry, InDeltaTime);
}

void UWidgetWebCamera::OnUpdateWebCameraTexture_Implementation(UTexture2D* InVideoTexture)
{
	if (Image_WebCamera->IsVisible() && InVideoTexture)
	{
		Image_WebCamera->SetBrushFromTexture(InVideoTexture);
	}
}

void UWidgetWebCamera::OpenWebCamera_Implementation(const FString& InCameraURL)
{
	CameraURL = InCameraURL;
	// Open the stream
	stream.open(TCHAR_TO_UTF8(*CameraURL));
	if (stream.isOpened())
	{
		// Initialize stream
		isStreamOpen = true;
		UpdateFrame();
		VideoSize = FVector2D(frame.cols, frame.rows);

		if (VideoSize.IsZero())
		{
			isStreamOpen = false;
			return;
		}

		VideoTexture = UTexture2D::CreateTransient(VideoSize.X, VideoSize.Y);

		VideoTexture->UpdateResource();
		VideoUpdateTextureRegion = new FUpdateTextureRegion2D(0, 0, 0, 0, VideoSize.X, VideoSize.Y);

		// Initialize data array
		Data.Init(FColor(0, 0, 0, 255), VideoSize.X * VideoSize.Y);

		// Do first frame
		UpdateTexture();
		OnUpdateWebCameraTexture(VideoTexture);
	}
	if (isStreamOpen)
	{
		OnOpenWebcamSuccessfully.Broadcast(CameraURL);
	}
	else
	{
		OnOpenWebcamFailed.Broadcast(CameraURL);
	}
}

void UWidgetWebCamera::UpdateFrame()
{
	if (stream.isOpened())
	{
		if (stream.read(frame) && frame.data) {
			if (ShouldResize)
			{
				cv::resize(frame, frame, cv::Size(ResizeDeminsions.X, ResizeDeminsions.Y));
			}
			return;
		}
	}
}

void UWidgetWebCamera::UpdateTexture()
{
	if (isStreamOpen && frame.data)
	{
		// Copy Mat data to Data array
		for (int y = 0; y < VideoSize.Y; y++)
		{
			for (int x = 0; x < VideoSize.X; x++)
			{
				int i = x + (y * VideoSize.X);
				Data[i].B = frame.data[i * 3 + 0];
				Data[i].G = frame.data[i * 3 + 1];
				Data[i].R = frame.data[i * 3 + 2];
			}
		}

		// Update texture 2D
		UpdateTextureRegions(VideoTexture, (int32)0, (uint32)1, VideoUpdateTextureRegion, (uint32)(4 * VideoSize.X), (uint32)4, (uint8*)Data.GetData(), false);
	}
}

void UWidgetWebCamera::UpdateTextureRegions(UTexture2D* Texture, int32 MipIndex, uint32 NumRegions, FUpdateTextureRegion2D* Regions, uint32 SrcPitch, uint32 SrcBpp, uint8* SrcData, bool bFreeData)
{
	if (Texture->Resource)
	{
		struct FUpdateTextureRegionsData
		{
			FTexture2DResource* Texture2DResource;
			int32 MipIndex;
			uint32 NumRegions;
			FUpdateTextureRegion2D* Regions;
			uint32 SrcPitch;
			uint32 SrcBpp;
			uint8* SrcData;
		};

		FUpdateTextureRegionsData* RegionData = new FUpdateTextureRegionsData;

		RegionData->Texture2DResource = (FTexture2DResource*)Texture->Resource;
		RegionData->MipIndex = MipIndex;
		RegionData->NumRegions = NumRegions;
		RegionData->Regions = Regions;
		RegionData->SrcPitch = SrcPitch;
		RegionData->SrcBpp = SrcBpp;
		RegionData->SrcData = SrcData;

		ENQUEUE_RENDER_COMMAND(UpdateTextureRegionsData)(
			[RegionData, bFreeData](FRHICommandListImmediate& RHICmdList)
		{
			for (uint32 RegionIndex = 0; RegionIndex < RegionData->NumRegions; ++RegionIndex)
			{
				int32 CurrentFirstMip = RegionData->Texture2DResource->GetCurrentFirstMip();
				if (RegionData->MipIndex >= CurrentFirstMip)
				{
					RHIUpdateTexture2D(
						RegionData->Texture2DResource->GetTexture2DRHI(),
						RegionData->MipIndex - CurrentFirstMip,
						RegionData->Regions[RegionIndex],
						RegionData->SrcPitch,
						RegionData->SrcData
						+ RegionData->Regions[RegionIndex].SrcY * RegionData->SrcPitch
						+ RegionData->Regions[RegionIndex].SrcX * RegionData->SrcBpp
					);
				}
			}
			if (bFreeData)
			{
				FMemory::Free(RegionData->Regions);
				FMemory::Free(RegionData->SrcData);
			}
			delete RegionData;
		});
	}
}