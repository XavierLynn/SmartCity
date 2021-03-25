// Copyright 2020 byTangs, All Rights Reserved.

#include "EngineFramework/FileManager/TextureRuntimeImporter/TextureImporter.h"
#include "DDSLoader.h"
#include "Misc/Paths.h"
#include "IImageWrapperModule.h"
#include "Misc/FileHelper.h"
#include "IImageWrapper.h"
#include "PSD/PSDTextureHelper.h"
#include "TGA/TGATextureHelper.h"
#include "TIF/TIFTexureHelper.h"
#include "PCX/PCXTextureHelper.h"
#include "Modules/ModuleManager.h"
#include "HAL/UnrealMemory.h"
#include "Engine/Texture2D.h"

DEFINE_LOG_CATEGORY(RuntimeTextureImport)

EME_START

TSharedPtr<FTextureImporter> FTextureImporter::Singleton;

FTextureImporter::~FTextureImporter()
{
	if (Singleton.IsValid())
	{
		Singleton.Reset();
	}
}

int32 FTextureImporter::GetBytesPerPixel(ETextureSourceFormat Format)
{
	int32 BytesPerPixel = 0;
	switch (Format)
	{
	case TSF_G8:		BytesPerPixel = 1; break;
	case TSF_G16:		BytesPerPixel = 2; break;
	case TSF_BGRA8:		BytesPerPixel = 4; break;
	case TSF_BGRE8:		BytesPerPixel = 4; break;
	case TSF_RGBA16:	BytesPerPixel = 8; break;
	case TSF_RGBA16F:	BytesPerPixel = 8; break;
	default:			BytesPerPixel = 0; break;
	}
	return BytesPerPixel;
}

UTexture2D* FTextureImporter::ImportTexture(const FString& Filename)
{
	if (Filename.IsEmpty() || !FPaths::FileExists(Filename))
	{
		RTI_LOG(Error, TEXT("Filename is empty or file does not exist..."));
		return nullptr;
	}

	TArray<uint8> Buffer;
	if (FFileHelper::LoadFileToArray(Buffer, *Filename))
	{
		return CreateUTexture2DByBuffer(Buffer);
	}

	return nullptr;
}

UTexture2D* EMEngine::FTextureImporter::CreateUTexture2DByBuffer(TArray<uint8>& Buffer)
{
	IImageWrapperModule* ImageWrapperModule = FModuleManager::Get().LoadModulePtr<IImageWrapperModule>(TEXT("ImageWrapper"));
	if (ImageWrapperModule == nullptr)
	{
		RTI_LOG(Error, TEXT("can not load IImageWrapperModule ... "));
		return  nullptr;
	}

	EPixelFormat PixelFormat = PF_Unknown;

	// detect image format
	TSharedPtr<IImageWrapper> PngImageWrapper = ImageWrapperModule->CreateImageWrapper(EImageFormat::PNG);
	if (PngImageWrapper.IsValid() && PngImageWrapper->SetCompressed(Buffer.GetData(), Buffer.GetAllocatedSize()))
	{
		int32 BitDepth = PngImageWrapper->GetBitDepth();
		ERGBFormat Format = PngImageWrapper->GetFormat();

		if (Format == ERGBFormat::Gray)
		{
			if (BitDepth <= 8)
			{
				Format = ERGBFormat::Gray;
				PixelFormat = PF_B8G8R8A8;
				BitDepth = 8;
			}
			else if (BitDepth == 16)
			{
				// TODO: TSF_G16?
				Format = ERGBFormat::RGBA;
				BitDepth = 16;
				PixelFormat = PF_FloatRGBA;
			}
		}
		else if (Format == ERGBFormat::RGBA || Format == ERGBFormat::BGRA)
		{
			if (BitDepth <= 8)
			{
				Format = ERGBFormat::BGRA;
				PixelFormat = PF_B8G8R8A8;
				BitDepth = 8;
			}
			else if (BitDepth == 16)
			{
				Format = ERGBFormat::RGBA;
				BitDepth = 16;
				PixelFormat = PF_FloatRGBA;
			}
		}

		if (PixelFormat == PF_Unknown)  //Invalid png format
		{
			RTI_LOG(Error, TEXT("PNG file contains data in an unsupported format."));
			return nullptr;
		}

		TArray64<uint8> UncompressedData;
		PngImageWrapper->GetRaw(Format, BitDepth, UncompressedData);

		UTexture2D* NewTexture = UTexture2D::CreateTransient(PngImageWrapper->GetWidth(), PngImageWrapper->GetHeight(), PixelFormat);
		if (NewTexture)
		{
			uint8* MipData = static_cast<uint8*>(NewTexture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE));

			// Bulk data was already allocated for the correct size when we called CreateTransient above
			FMemory::Memcpy(MipData, UncompressedData.GetData(), NewTexture->PlatformData->Mips[0].BulkData.GetBulkDataSize());

			NewTexture->PlatformData->Mips[0].BulkData.Unlock();

			NewTexture->UpdateResource();
		}
		return NewTexture;
	}

	//detect JPEG
	TSharedPtr<IImageWrapper> JpegImageWrapper = ImageWrapperModule->CreateImageWrapper(EImageFormat::JPEG);
	if (JpegImageWrapper.IsValid() && JpegImageWrapper->SetCompressed(Buffer.GetData(), Buffer.GetAllocatedSize()))
	{
		int32 BitDepth = JpegImageWrapper->GetBitDepth();
		ERGBFormat Format = JpegImageWrapper->GetFormat();

		if (Format == ERGBFormat::Gray)
		{
			if (BitDepth <= 8)
			{
				PixelFormat = PF_B8G8R8A8;
				Format = ERGBFormat::Gray;
				BitDepth = 8;
			}
		}
		else if (Format == ERGBFormat::RGBA)
		{
			if (BitDepth <= 8)
			{
				PixelFormat = PF_B8G8R8A8;
				Format = ERGBFormat::BGRA;
				BitDepth = 8;
			}
		}

		if (PixelFormat == PF_Unknown)
		{
			RTI_LOG(Error, TEXT("JPEG file contains data in an unsupported format."));
			return nullptr;
		}

		TArray64<uint8> UncompressedData;
		JpegImageWrapper->GetRaw(Format, BitDepth, UncompressedData);

		UTexture2D* NewTexture = UTexture2D::CreateTransient(JpegImageWrapper->GetWidth(), JpegImageWrapper->GetHeight(), PixelFormat);
		if (NewTexture)
		{
			uint8* MipData = static_cast<uint8*>(NewTexture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE));

			// Bulk data was already allocated for the correct size when we called CreateTransient above
			FMemory::Memcpy(MipData, UncompressedData.GetData(), NewTexture->PlatformData->Mips[0].BulkData.GetBulkDataSize());

			NewTexture->PlatformData->Mips[0].BulkData.Unlock();

			NewTexture->UpdateResource();
		}
		return NewTexture;
	}

	//detech EXR
	TSharedPtr<IImageWrapper> ExrImageWrapper = ImageWrapperModule->CreateImageWrapper(EImageFormat::EXR);
	if (ExrImageWrapper.IsValid() && ExrImageWrapper->SetCompressed((void*)Buffer.GetData(), Buffer.GetAllocatedSize()))
	{
		const int32 Width = ExrImageWrapper->GetWidth();
		const int32 Height = ExrImageWrapper->GetHeight();

		int32 BitDepth = ExrImageWrapper->GetBitDepth();
		ERGBFormat Format = ExrImageWrapper->GetFormat();

		if (Format == ERGBFormat::RGBA && BitDepth == 16)
		{
			PixelFormat = PF_A16B16G16R16;
			Format = ERGBFormat::BGRA;
		}
		if (PixelFormat == PF_Unknown)
		{
			RTI_LOG(Error, TEXT("EXR file contains data in an unsupported format."));
			return nullptr;
		}

		TArray64<uint8> UncompressedData;
		ExrImageWrapper->GetRaw(Format, BitDepth, UncompressedData);

		UTexture2D* NewTexture = UTexture2D::CreateTransient(Width, Height, PixelFormat);
		if (NewTexture)
		{
			uint8* MipData = static_cast<uint8*>(NewTexture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE));

			// Bulk data was already allocated for the correct size when we called CreateTransient above
			FMemory::Memcpy(MipData, UncompressedData.GetData(), NewTexture->PlatformData->Mips[0].BulkData.GetBulkDataSize());

			NewTexture->PlatformData->Mips[0].BulkData.Unlock();
			NewTexture->UpdateResource();
		}
		return NewTexture;
	}

	//TGA
	const FTGAFileHeader* TGA = (FTGAFileHeader*)Buffer.GetData();

	if (Buffer.Num() >= sizeof(FTGAFileHeader) &&
		((TGA->ColorMapType == 0 && TGA->ImageTypeCode == 2) ||
			// ImageTypeCode 3 is greyscale
			(TGA->ColorMapType == 0 && TGA->ImageTypeCode == 3) ||
			(TGA->ColorMapType == 0 && TGA->ImageTypeCode == 10) ||
			(TGA->ColorMapType == 1 && TGA->ImageTypeCode == 1 && TGA->BitsPerPixel == 8)))
	{
		TArray64<uint8> Data;
		const bool bResult = DecompressTGA(TGA, Data);
		if (bResult)
		{
			PixelFormat = PF_B8G8R8A8;
			UTexture2D* NewTexture = UTexture2D::CreateTransient(TGA->Width, TGA->Height, PixelFormat);
			if (NewTexture)
			{
				uint8* MipData = static_cast<uint8*>(NewTexture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE));

				// Bulk data was already allocated for the correct size when we called CreateTransient above
				FMemory::Memcpy(MipData, Data.GetData(), NewTexture->PlatformData->Mips[0].BulkData.GetBulkDataSize());

				NewTexture->PlatformData->Mips[0].BulkData.Unlock();
				NewTexture->UpdateResource();
			}
			return NewTexture;
		}
		return nullptr;
	}

	//PSD
	FPSDFileHeader			 psdhdr;
	if (Buffer.Num() > sizeof(FPSDFileHeader))
	{
		GetPSDHeader(Buffer.GetData(), psdhdr);
	}
	if (psdhdr.IsValid())
	{
		if (!psdhdr.IsSupported())
		{
			RTI_LOG(Error, TEXT("Format of this PSD is not supported. Only Grayscale and RGBColor PSD images are currently supported, in 8-bit or 16-bit."));
			return nullptr;
		}

		// Select the texture's source format
		ETextureSourceFormat TextureFormat = TSF_Invalid;
		if (psdhdr.Depth == 8)
		{
			TextureFormat = TSF_BGRA8;
			PixelFormat = PF_B8G8R8A8;
		}
		else if (psdhdr.Depth == 16)
		{
			TextureFormat = TSF_RGBA16;
			PixelFormat = PF_FloatRGBA;
		}

		if (TextureFormat == TSF_Invalid)
		{
			RTI_LOG(Error, TEXT("PSD file contains data in an unsupported format."));
			return nullptr;
		}

		TArray64<uint8> OutData;
		OutData.AddUninitialized((int64)psdhdr.Width * psdhdr.Height * GetBytesPerPixel(TextureFormat));

		uint8* Dst = (uint8*)OutData.GetData();
		const uint8* Data = Buffer.GetData();
		if (!ReadPSDData(Dst, Data, psdhdr))
		{
			RTI_LOG(Error, TEXT("Failed to read this PSD"));
			return nullptr;
		}

		UTexture2D* NewTexture = UTexture2D::CreateTransient(psdhdr.Width, psdhdr.Height, PixelFormat);
		if (NewTexture)
		{
			uint8* MipData = static_cast<uint8*>(NewTexture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE));

			// Bulk data was already allocated for the correct size when we called CreateTransient above
			FMemory::Memcpy(MipData, OutData.GetData(), NewTexture->PlatformData->Mips[0].BulkData.GetBulkDataSize());

			NewTexture->PlatformData->Mips[0].BulkData.Unlock();
			NewTexture->UpdateResource();
		}
		return NewTexture;
	}

	//DDS Texture
	FDDSLoadHelper  DDSLoadHelper(Buffer.GetData(), Buffer.Num());

	if (DDSLoadHelper.IsValid2DTexture())
	{
		ETextureSourceFormat SourceFormat = DDSLoadHelper.ComputeSourceFormat();
		// Invalid DDS format
		if (SourceFormat == TSF_Invalid)
		{
			RTI_LOG(Error, TEXT("DDS uses an unsupported format."));
			return nullptr;
		}

		uint32 MipMapCount = DDSLoadHelper.ComputeMipMapCount();
		if (SourceFormat != TSF_Invalid && MipMapCount > 0)
		{
			TArray64<uint8> OutData;
			OutData.AddUninitialized((int64)DDSLoadHelper.DDSHeader->dwWidth * DDSLoadHelper.DDSHeader->dwHeight * GetBytesPerPixel(SourceFormat));
			FMemory::Memcpy(OutData.GetData(), DDSLoadHelper.GetDDSDataPointer(), OutData.Num());

			switch (SourceFormat)
			{
			case TSF_Invalid:
				PixelFormat = PF_Unknown;
				break;
			case TSF_G8:
				PixelFormat = PF_Unknown;
				break;
			case TSF_BGRA8:
				PixelFormat = PF_B8G8R8A8;
				break;
			case TSF_BGRE8:
				PixelFormat = PF_Unknown;
				break;
			case TSF_RGBA16:
				PixelFormat = PF_FloatRGBA;
				break;
			case TSF_RGBA16F:
				PixelFormat = PF_FloatRGBA;
				break;
			case TSF_RGBA8:
				PixelFormat = PF_A8R8G8B8;
				break;
			case TSF_RGBE8:
				PixelFormat = PF_Unknown;
				break;
			case TSF_G16:
				PixelFormat = PF_Unknown;
				break;
			case TSF_MAX:
				PixelFormat = PF_Unknown;
				break;
			default:
				PixelFormat = PF_Unknown;
				break;
			}

			if (PixelFormat == PF_Unknown) return nullptr;

			UTexture2D* NewTexture = UTexture2D::CreateTransient(DDSLoadHelper.DDSHeader->dwWidth, DDSLoadHelper.DDSHeader->dwHeight, PixelFormat);
			if (NewTexture)
			{
				uint8* MipData = static_cast<uint8*>(NewTexture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE));

				// Bulk data was already allocated for the correct size when we called CreateTransient above
				FMemory::Memcpy(MipData, OutData.GetData(), NewTexture->PlatformData->Mips[0].BulkData.GetBulkDataSize());

				NewTexture->PlatformData->Mips[0].BulkData.Unlock();
				NewTexture->UpdateResource();
			}
			return NewTexture;
		}
		return nullptr;
	}

	//Tif
	FTifTextureHelper TiffLoaderHelper;

	if (TiffLoaderHelper.IsValid())
	{
		if (TiffLoaderHelper.Load(Buffer.GetData(), Buffer.Num()))
		{
			switch (TiffLoaderHelper.TextureSourceFormat)
			{
			case TSF_Invalid:
				PixelFormat = PF_Unknown;
				break;
			case TSF_G8:
				PixelFormat = PF_Unknown;
				break;
			case TSF_BGRA8:
				PixelFormat = PF_B8G8R8A8;
				break;
			case TSF_BGRE8:
				PixelFormat = PF_Unknown;
				break;
			case TSF_RGBA16:
				PixelFormat = PF_FloatRGBA;
				break;
			case TSF_RGBA16F:
				PixelFormat = PF_FloatRGBA;
				break;
			case TSF_RGBA8:
				PixelFormat = PF_A8R8G8B8;
				break;
			case TSF_RGBE8:
				PixelFormat = PF_Unknown;
				break;
			case TSF_G16:
				PixelFormat = PF_Unknown;
				break;
			case TSF_MAX:
				PixelFormat = PF_Unknown;
				break;
			default:
				PixelFormat = PF_Unknown;
				break;
			}

			if (PixelFormat == PF_Unknown)
			{
				RTI_LOG(Error, TEXT("Tif uses an unsupported format. "));
				return nullptr;
			}

			TArray64<uint8> OutData;
			OutData.AddUninitialized((int64)TiffLoaderHelper.Width * TiffLoaderHelper.Height * GetBytesPerPixel(TiffLoaderHelper.TextureSourceFormat));
			FMemory::Memcpy(OutData.GetData(), TiffLoaderHelper.RawData.GetData(), OutData.Num());

			UTexture2D* NewTexture = UTexture2D::CreateTransient(TiffLoaderHelper.Width, TiffLoaderHelper.Height, PixelFormat);
			if (NewTexture)
			{
				uint8* MipData = static_cast<uint8*>(NewTexture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE));

				// Bulk data was already allocated for the correct size when we called CreateTransient above
				FMemory::Memcpy(MipData, OutData.GetData(), NewTexture->PlatformData->Mips[0].BulkData.GetBulkDataSize());

				NewTexture->PlatformData->Mips[0].BulkData.Unlock();

				NewTexture->UpdateResource();
			}
			return NewTexture;
		}
	}

	//BMP

	TSharedPtr<IImageWrapper> BmpImageWrapper = ImageWrapperModule->CreateImageWrapper(EImageFormat::BMP);
	if (BmpImageWrapper.IsValid() && BmpImageWrapper->SetCompressed(Buffer.GetData(), Buffer.Num()))
	{
		PixelFormat = PF_B8G8R8A8;

		if (PixelFormat == PF_Unknown)
		{
			RTI_LOG(Error, TEXT("BMP uses an unsupported format. "));
			return nullptr;
		}

		TArray64<uint8> OutData;
		OutData.AddUninitialized((int64)BmpImageWrapper->GetWidth() * BmpImageWrapper->GetHeight() * GetBytesPerPixel(TSF_BGRA8));
		if (BmpImageWrapper->GetRaw(BmpImageWrapper->GetFormat(), BmpImageWrapper->GetBitDepth(), OutData))
		{
			UTexture2D* NewTexture = UTexture2D::CreateTransient(BmpImageWrapper->GetWidth(), BmpImageWrapper->GetHeight(), PixelFormat);
			if (NewTexture)
			{
				uint8* MipData = static_cast<uint8*>(NewTexture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE));

				// Bulk data was already allocated for the correct size when we called CreateTransient above
				FMemory::Memcpy(MipData, OutData.GetData(), NewTexture->PlatformData->Mips[0].BulkData.GetBulkDataSize());

				NewTexture->PlatformData->Mips[0].BulkData.Unlock();

				NewTexture->UpdateResource();
			}
			return NewTexture;
		}
	}

	//PCX
	const FPCXFileHeader* PCX = (FPCXFileHeader*)Buffer.GetData();
	const uint8* PCXData = Buffer.GetData();
	int32 Length = Buffer.Num();

	if (Buffer.Num() >= sizeof(FPCXFileHeader) && PCX->Manufacturer == 10)
	{
		int32 NewU = PCX->XMax + 1 - PCX->XMin;
		int32 NewV = PCX->YMax + 1 - PCX->YMin;

		if (PCX->NumPlanes == 1 && PCX->BitsPerPixel == 8)
		{
			TArray64<uint8> OutData;
			OutData.AddUninitialized((int64)NewU * NewV * GetBytesPerPixel(TSF_BGRA8));
			FColor* DestPtr = (FColor*)OutData.GetData();

			// Import the palette.
			uint8* PCXPalette = (uint8*)(PCXData + Length - 256 * 3);
			TArray<FColor>	Palette;
			for (uint32 i = 0; i < 256; i++)
			{
				Palette.Add(FColor(PCXPalette[i * 3 + 0], PCXPalette[i * 3 + 1], PCXPalette[i * 3 + 2], i == 0 ? 0 : 255));
			}

			// Import it.
			FColor* DestEnd = DestPtr + NewU * NewV;
			PCXData += 128;
			while (DestPtr < DestEnd)
			{
				uint8 Color = *PCXData++;
				if ((Color & 0xc0) == 0xc0)
				{
					uint32 RunLength = Color & 0x3f;
					Color = *PCXData++;

					for (uint32 Index = 0; Index < RunLength; Index++)
					{
						*DestPtr++ = Palette[Color];
					}
				}
				else *DestPtr++ = Palette[Color];
			}

			UTexture2D* NewTexture = UTexture2D::CreateTransient(NewU, NewV, PF_B8G8R8A8);
			if (NewTexture)
			{
				uint8* MipData = static_cast<uint8*>(NewTexture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE));

				// Bulk data was already allocated for the correct size when we called CreateTransient above
				FMemory::Memcpy(MipData, OutData.GetData(), NewTexture->PlatformData->Mips[0].BulkData.GetBulkDataSize());

				NewTexture->PlatformData->Mips[0].BulkData.Unlock();
				NewTexture->UpdateResource();
			}
			return NewTexture;
		}
		else if (PCX->NumPlanes == 3 && PCX->BitsPerPixel == 8)
		{
			TArray64<uint8> OutData;
			OutData.AddUninitialized((int64)NewU * NewV * GetBytesPerPixel(TSF_BGRA8));
			uint8* Dest = OutData.GetData();

			// Doing a memset to make sure the alpha channel is set to 0xff since we only have 3 color planes.
			FMemory::Memset(Dest, 0xff, NewU * NewV * GetBytesPerPixel(TSF_BGRA8));
			// Copy upside-down scanlines.
			PCXData += 128;
			int32 CountU = FMath::Min<int32>(PCX->BytesPerLine, NewU);
			for (int32 i = 0; i < NewV; i++)
			{
				// We need to decode image one line per time building RGB image color plane by color plane.
				int32 RunLength, Overflow = 0;
				uint8 Color = 0;
				for (int32 ColorPlane = 2; ColorPlane >= 0; ColorPlane--)
				{
					for (int32 j = 0; j < CountU; j++)
					{
						if (!Overflow)
						{
							Color = *PCXData++;
							if ((Color & 0xc0) == 0xc0)
							{
								RunLength = FMath::Min((Color & 0x3f), CountU - j);
								Overflow = (Color & 0x3f) - RunLength;
								Color = *PCXData++;
							}
							else
								RunLength = 1;
						}
						else
						{
							RunLength = FMath::Min(Overflow, CountU - j);
							Overflow = Overflow - RunLength;
						}

						//checkf(((i*NewU + RunLength) * 4 + ColorPlane) < (Texture->Source.CalcMipSize(0)),
						//	TEXT("RLE going off the end of buffer"));
						for (int32 k = j; k < j + RunLength; k++)
						{
							Dest[(i * NewU + k) * 4 + ColorPlane] = Color;
						}
						j += RunLength - 1;
					}
				}
			}

			UTexture2D* NewTexture = UTexture2D::CreateTransient(NewU, NewV, PF_B8G8R8A8);
			if (NewTexture)
			{
				uint8* MipData = static_cast<uint8*>(NewTexture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE));

				// Bulk data was already allocated for the correct size when we called CreateTransient above
				FMemory::Memcpy(MipData, Dest, NewTexture->PlatformData->Mips[0].BulkData.GetBulkDataSize());

				NewTexture->PlatformData->Mips[0].BulkData.Unlock();
				NewTexture->UpdateResource();
			}
			return NewTexture;
		}
		else
		{
			RTI_LOG(Error, TEXT("PCX uses an unsupported format (%i/%i)"), PCX->NumPlanes, PCX->BitsPerPixel);
			return nullptr;
		}
	}
	return nullptr;
}

EME_END