// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncTaskImageLoader.h"
#include "Async/Async.h"
#include "Engine/Texture2D.h"
#include "Misc/FileHelper.h"
#include "IImageWrapper.h"
#include "Blueprint/AsyncTaskDownloadImage.h"
#include "Modules/ModuleManager.h"
#include "Engine/Texture2DDynamic.h"
#include "IImageWrapperModule.h"

#define UIL_LOG(Verbosity, Format, ...)	UE_LOG(LogTemp, Verbosity, Format, __VA_ARGS__)

#if !UE_SERVER

static void WriteRawToTexture_RenderThread(FTexture2DDynamicResource* TextureResource, const TArray<uint8>& RawData, bool bUseSRGB = true)
{
	check(IsInRenderingThread());

	FRHITexture2D* TextureRHI = TextureResource->GetTexture2DRHI();

	int32 Width = TextureRHI->GetSizeX();
	int32 Height = TextureRHI->GetSizeY();

	uint32 DestStride = 0;
	uint8* DestData = reinterpret_cast<uint8*>(RHILockTexture2D(TextureRHI, 0, RLM_WriteOnly, DestStride, false, false));

	for (int32 y = 0; y < Height; y++)
	{
		uint8* DestPtr = &DestData[(Height - 1 - y) * DestStride];

		const FColor* SrcPtr = &((FColor*)(RawData.GetData()))[(Height - 1 - y) * Width];
		for (int32 x = 0; x < Width; x++)
		{
			*DestPtr++ = SrcPtr->B;
			*DestPtr++ = SrcPtr->G;
			*DestPtr++ = SrcPtr->R;
			*DestPtr++ = SrcPtr->A;
			SrcPtr++;
		}
	}

	RHIUnlockTexture2D(TextureRHI, 0, false, false);
}

#endif

UAsyncTaskImageLoader* UAsyncTaskImageLoader::DownloadOrLoadImageFromDisk(FString URL)
{
	UAsyncTaskImageLoader* DownloadTask = NewObject<UAsyncTaskImageLoader>();
	if(URL.StartsWith("http"))
	{
		DownloadTask->Start(URL);
		return DownloadTask;
	}
	else if(URL.StartsWith("file:///"))
	{
		DownloadTask->LoadFromDiskAsync(URL.Mid(8));
		return DownloadTask;
	}
	else
	{
		DownloadTask->OnFail.Broadcast(nullptr);
		DownloadTask->RemoveFromRoot();
		Async(EAsyncExecution::ThreadPool, [=]() {},
			[=]() {
				AsyncTask(ENamedThreads::GameThread, [=]()
					{
						DownloadTask->OnFail.Broadcast(nullptr);
						return;
					});
			});
		return DownloadTask;
	}
	
}

void UAsyncTaskImageLoader::LoadFromDiskAsync(FString URL)
{
	RemoveFromRoot();
	TFuture<bool> Future = Async(EAsyncExecution::ThreadPool, [=]() {LoadFromDiskSuccess = LoadFromDisk(URL); return LoadFromDiskSuccess; },
		[this](){
		AsyncTask(ENamedThreads::GameThread, [this]()
			{
				if (LoadFromDiskSuccess == false)
				{
						OnFail.Broadcast(nullptr);
				}
				return;
			});
	});
}

bool UAsyncTaskImageLoader::LoadFromDisk(const FString& ImagePath)
{
	if (!FPaths::FileExists(ImagePath))
	{
		UIL_LOG(Error, TEXT("File not found: %s"), *ImagePath);
		return false;
	}
	TArray<uint8> FileData;
	if (!FFileHelper::LoadFileToArray(FileData, *ImagePath))
	{
		UIL_LOG(Error, TEXT("Failed to load file: %s"), *ImagePath);
		return false;
	}
	static IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	EImageFormat ImageFormat = ImageWrapperModule.DetectImageFormat(FileData.GetData(), FileData.Num());
	if (ImageFormat == EImageFormat::Invalid)
	{
		UIL_LOG(Error, TEXT("Unrecognized image file format: %s"), *ImagePath);
		return false;
	}

	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(ImageFormat);
	if (!ImageWrapper.IsValid())
	{
		UIL_LOG(Error, TEXT("Failed to create image wrapper for file: %s"), *ImagePath);
		return false;
	}

	TArray<uint8> RawData;
	ImageWrapper->SetCompressed(FileData.GetData(), FileData.Num());
	const ERGBFormat InFormat = ERGBFormat::BGRA;
	ImageWrapper->GetRaw(InFormat, 8, RawData);
	if (RawData.Num() == 0)
	{
		UIL_LOG(Error, TEXT("Failed to decompress image file: %s"), *ImagePath);
		return false;
	}
	
	FString TextureBaseName = TEXT("Texture_") + FPaths::GetBaseFilename(ImagePath);
	AsyncTask(ENamedThreads::GameThread, [=]()
		{
			if (UTexture2DDynamic* Texture = UTexture2DDynamic::Create(ImageWrapper->GetWidth(), ImageWrapper->GetHeight()))
			{
				Texture->SRGB = true;
				Texture->UpdateResource();

				FTexture2DDynamicResource* TextureResource = static_cast<FTexture2DDynamicResource*>(Texture->Resource);
				//TArray<uint8> RawDataCopy = *RawData;
				ENQUEUE_RENDER_COMMAND(FWriteRawDataToTexture)(
					[TextureResource, RawData](FRHICommandListImmediate& RHICmdList)
					{
						WriteRawToTexture_RenderThread(TextureResource, RawData);
					});
				OnSuccess.Broadcast(Texture);
				return;
			}
			else
			{
				OnFail.Broadcast(nullptr);
			}
		});
	return true;
}