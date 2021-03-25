// Copyright 2020 byTangs, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "TextureRuntimeImporterCore.h"

EME_START
#pragma pack(push,1)

struct FPSDFileHeader
{
    int32     Signature;        // 8BPS
    int16     Version;          // Version
    int16     nChannels;        // Number of Channels (3=RGB) (4=RGBA)
    int32     Height;           // Number of Image Rows
    int32     Width;            // Number of Image Columns
    int16     Depth;            // Number of Bits per Channel
    int16     Mode;             // Image Mode (0=Bitmap)(1=Grayscale)(2=Indexed)(3=RGB)(4=CYMK)(7=Multichannel)
    uint8     Pad[6];           // Padding

    /**
    * @return Whether file has a valid signature
    */
    bool IsValid(void)const
    {
        // Fail on bad signature
        if (Signature != 0x38425053)
            return false;

        return true;
    }

    /**
    * @return Whether file has a supported version
    */
    bool IsSupported(void)const
    {
        // Fail on bad version
        if (Version != 1)
            return false;
        // Fail on anything other than 1, 3 or 4 channels
        if ((nChannels != 1) && (nChannels != 3) && (nChannels != 4))
            return false;
        // Fail on anything other than 8 Bits/channel or 16 Bits/channel  
        if ((Depth != 8) && (Depth != 16))
            return false;
        // Fail on anything other than Grayscale and RGB
        // We can add support for indexed later if needed.
        if (Mode != 1 && Mode != 3)
            return false;

        return true;
    }
};

EMENGINE_API void GetPSDHeader(const uint8* Buffer, FPSDFileHeader& Info);
EMENGINE_API bool ReadPSDData(uint8* OutData, const uint8*& Buffer, FPSDFileHeader& Info);

#pragma pack(pop)
EME_END