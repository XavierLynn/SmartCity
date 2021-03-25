// Copyright 2020 byTangs, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "TextureRuntimeImporterCore.h"

EME_START

#pragma pack(push,1)

//TGA
struct FTGAFileHeader
{
    uint8  IdFieldLength;
    uint8  ColorMapType;
    uint8  ImageTypeCode;		// 2 for uncompressed RGB format
    uint16 ColorMapOrigin;
    uint16 ColorMapLength;
    uint8  ColorMapEntrySize;
    uint16 XOrigin;
    uint16 YOrigin;
    uint16 Width;
    uint16 Height;
    uint8  BitsPerPixel;
    uint8  ImageDescriptor;
    friend FArchive& operator<<(FArchive& Ar, FTGAFileHeader& H)
    {
        Ar << H.IdFieldLength << H.ColorMapType << H.ImageTypeCode;
        Ar << H.ColorMapOrigin << H.ColorMapLength << H.ColorMapEntrySize;
        Ar << H.XOrigin << H.YOrigin << H.Width << H.Height << H.BitsPerPixel;
        Ar << H.ImageDescriptor;
        return Ar;
    }
};


EMENGINE_API bool DecompressTGA_Helper(const FTGAFileHeader* TGA, uint32*& TextureData, const int32 TextureDataSize);
EMENGINE_API bool DecompressTGA(const FTGAFileHeader* TGA, TArray64<uint8>& InBuffer);
EMENGINE_API void DecompressTGA_32bpp(const FTGAFileHeader* TGA, uint32* TextureData);
EMENGINE_API void DecompressTGA_24bpp(const FTGAFileHeader* TGA, uint32* TextureData);
EMENGINE_API void DecompressTGA_16bpp(const FTGAFileHeader* TGA, uint32* TextureData);
EMENGINE_API void DecompressTGA_8bpp(const FTGAFileHeader* TGA, uint8* TextureData);
EMENGINE_API void DecompressTGA_RLE_32bpp(const FTGAFileHeader* TGA, uint32* TextureData);
EMENGINE_API void DecompressTGA_RLE_24bpp(const FTGAFileHeader* TGA, uint32* TextureData);
EMENGINE_API void DecompressTGA_RLE_16bpp(const FTGAFileHeader* TGA, uint32* TextureData);


#pragma pack(pop)
EME_END