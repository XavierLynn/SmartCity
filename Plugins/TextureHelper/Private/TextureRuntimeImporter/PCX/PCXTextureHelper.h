// Copyright 2020 byTangs, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "TextureRuntimeImporterCore.h"

EME_START

#pragma pack(push,1)

class FPCXFileHeader
{
    
public:
    uint8	Manufacturer;		// Always 10.
    uint8	Version;			// PCX file version.
    uint8	Encoding;			// 1=run-length, 0=none.
    uint8	BitsPerPixel;		// 1,2,4, or 8.
    uint16	XMin;				// Dimensions of the image.
    uint16	YMin;				// Dimensions of the image.
    uint16	XMax;				// Dimensions of the image.
    uint16	YMax;				// Dimensions of the image.
    uint16	XDotsPerInch;		// Horizontal printer resolution.
    uint16	YDotsPerInch;		// Vertical printer resolution.
    uint8	OldColorMap[48];	// Old colormap info data.
    uint8	Reserved1;			// Must be 0.
    uint8	NumPlanes;			// Number of color planes (1, 3, 4, etc).
    uint16	BytesPerLine;		// Number of bytes per scanline.
    uint16	PaletteType;		// How to interpret palette: 1=color, 2=gray.
    uint16	HScreenSize;		// Horizontal monitor size.
    uint16	VScreenSize;		// Vertical monitor size.
    uint8	Reserved2[54];		// Must be 0.
    friend FArchive& operator<<(FArchive& Ar, FPCXFileHeader& H)
    {
        Ar << H.Manufacturer << H.Version << H.Encoding << H.BitsPerPixel;
        Ar << H.XMin << H.YMin << H.XMax << H.YMax << H.XDotsPerInch << H.YDotsPerInch;
        for (int32 i = 0; i < UE_ARRAY_COUNT(H.OldColorMap); i++)
            Ar << H.OldColorMap[i];
        Ar << H.Reserved1 << H.NumPlanes;
        Ar << H.BytesPerLine << H.PaletteType << H.HScreenSize << H.VScreenSize;
        for (int32 i = 0; i < UE_ARRAY_COUNT(H.Reserved2); i++)
            Ar << H.Reserved2[i];
        return Ar;
    }
};


#pragma pack(pop)

EME_END