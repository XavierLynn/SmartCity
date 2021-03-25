// Copyright 2020 byTangs, All Rights Reserved.

#include "PSDTextureHelper.h"

BYTANGS_BEGIN

void GetPSDHeader(const uint8* Buffer, FPSDFileHeader& Info)
{
    Info.Signature = (static_cast<int32>(Buffer[0]) << 24) +
        (static_cast<int32>(Buffer[1]) << 16) +
        (static_cast<int32>(Buffer[2]) << 8) +
        (static_cast<int32>(Buffer[3]) << 0);
    
    Info.Version = (static_cast<int32>(Buffer[4]) << 8) +
        (static_cast<int32>(Buffer[5]) << 0);
    
    Info.nChannels = (static_cast<int32>(Buffer[12]) << 8) +
        (static_cast<int32>(Buffer[13]) << 0);
    
    Info.Height = (static_cast<int32>(Buffer[14]) << 24) +
        (static_cast<int32>(Buffer[15]) << 16) +
        (static_cast<int32>(Buffer[16]) << 8) +
        (static_cast<int32>(Buffer[17]) << 0);
    
    Info.Width = (static_cast<int32>(Buffer[18]) << 24) +
        (static_cast<int32>(Buffer[19]) << 16) +
        (static_cast<int32>(Buffer[20]) << 8) +
        (static_cast<int32>(Buffer[21]) << 0);
    
    Info.Depth = (static_cast<int32>(Buffer[22]) << 8) +
        (static_cast<int32>(Buffer[23]) << 0);
    
    Info.Mode = (static_cast<int32>(Buffer[24]) << 8) +
        (static_cast<int32>(Buffer[25]) << 0);
}

bool ReadPSDData(uint8* OutData, const uint8*& Buffer, FPSDFileHeader& Info)
{
	const uint8* pPlane = nullptr;
	const uint8* pRowTable = nullptr;
	int32 iPlane;
	int16 CompressionType;
	int32 iPixel;
	int32 iRow;
	int32 CompressedBytes;
	int32 iByte;
	int32 Count;
	uint8 Value;

	// Double check to make sure this is a valid request
	if (!Info.IsValid() || !Info.IsSupported())
	{
		return false;
	}

	const uint8* pCur = Buffer + sizeof(FPSDFileHeader);
	int32         NPixels = Info.Width * Info.Height;

	int32  ClutSize = (static_cast<int32>(pCur[0]) << 24) +
		(static_cast<int32>(pCur[1]) << 16) +
		(static_cast<int32>(pCur[2]) << 8) +
		(static_cast<int32>(pCur[3]) << 0);
	pCur += 4;
	pCur += ClutSize;

	// Skip Image Resource Section
	int32 ImageResourceSize = (static_cast<int32>(pCur[0]) << 24) +
		(static_cast<int32>(pCur[1]) << 16) +
		(static_cast<int32>(pCur[2]) << 8) +
		(static_cast<int32>(pCur[3]) << 0);
	pCur += 4 + ImageResourceSize;

	// Skip Layer and Mask Section
	int32 LayerAndMaskSize = (static_cast<int32>(pCur[0]) << 24) +
		(static_cast<int32>(pCur[1]) << 16) +
		(static_cast<int32>(pCur[2]) << 8) +
		(static_cast<int32>(pCur[3]) << 0);
	pCur += 4 + LayerAndMaskSize;

	// Determine number of bytes per pixel
	int32 BytesPerPixel = 3;
	const int32 BytesPerChannel = (Info.Depth / 8);
	switch (Info.Mode)
	{
	case 1: // 'GrayScale'
		BytesPerPixel = BytesPerChannel;
		break;
	case 2:
		BytesPerPixel = 1;
		return false;  // until we support indexed...
		break;
	case 3: // 'RGBColor'
		if (Info.nChannels == 3)
			BytesPerPixel = 3 * BytesPerChannel;
		else
			BytesPerPixel = 4 * BytesPerChannel;
		break;
	default:
		return false;
		break;
	}

	// Get Compression Type
	CompressionType = (static_cast<int32>(pCur[0]) << 8) + (static_cast<int32>(pCur[1]) << 0);
	pCur += 2;

	// Fail on 16 Bits/channel with RLE. This can occur when the file is not saved with 'Maximize Compatibility'. Compression doesn't appear to be standard.
	if (CompressionType == 1 && Info.Depth == 16)
	{
		return false;
	}

	// If no alpha channel, set alpha to opaque (255 or 65536).
	if (Info.nChannels != 4)
	{
		if (Info.Depth == 8)
		{
			const uint32 Channels = 4;
			const uint32 BufferSize = Info.Width * Info.Height * Channels * sizeof(uint8);
			FMemory::Memset(OutData, 0xff, BufferSize);
		}
		else if (Info.Depth == 16)
		{
			const uint32 Channels = 4;
			const uint32 BufferSize = Info.Width * Info.Height * Channels * sizeof(uint16);
			FMemory::Memset(OutData, 0xff, BufferSize);
		}
	}

	// Uncompressed?
	if (CompressionType == 0)
	{
		if (Info.Depth == 8)
		{
			FColor* Dest = (FColor*)OutData;
			for (int32 Pixel = 0; Pixel < NPixels; Pixel++)
			{
				if (Info.nChannels == 1)
				{
					Dest[Pixel].R = pCur[NPixels + Pixel];
					Dest[Pixel].G = pCur[NPixels + Pixel];
					Dest[Pixel].B = pCur[NPixels + Pixel];
				}
				else
				{
					Dest[Pixel].R = pCur[NPixels * 0 + Pixel];
					Dest[Pixel].G = pCur[NPixels * 1 + Pixel];
					Dest[Pixel].B = pCur[NPixels * 2 + Pixel];
					if (Info.nChannels == 4)
					{
						Dest[Pixel].A = pCur[NPixels * 3 + Pixel];
					}
				}
			}
		}
		else if (Info.Depth == 16)
		{
			uint32 SrcOffset = 0;

			if (Info.nChannels == 1)
			{
				uint16* Dest = (uint16*)OutData;
				uint32 ChannelOffset = 0;

				for (int32 Pixel = 0; Pixel < NPixels; Pixel++)
				{
					Dest[ChannelOffset + 0] = ((pCur[SrcOffset] << 8) + (pCur[SrcOffset + 1] << 0));
					Dest[ChannelOffset + 1] = ((pCur[SrcOffset] << 8) + (pCur[SrcOffset + 1] << 0));
					Dest[ChannelOffset + 2] = ((pCur[SrcOffset] << 8) + (pCur[SrcOffset + 1] << 0));

					//Increment offsets
					ChannelOffset += 4;
					SrcOffset += BytesPerChannel;
				}
			}
			else
			{
				// Loop through the planes	
				for (iPlane = 0; iPlane < Info.nChannels; iPlane++)
				{
					uint16* Dest = (uint16*)OutData;
					uint32 ChannelOffset = iPlane;

					for (int32 Pixel = 0; Pixel < NPixels; Pixel++)
					{
						Dest[ChannelOffset] = ((pCur[SrcOffset] << 8) + (pCur[SrcOffset + 1] << 0));

						//Increment offsets
						ChannelOffset += 4;
						SrcOffset += BytesPerChannel;
					}
				}
			}
		}
	}
	// RLE?
	else if (CompressionType == 1)
	{
		// Setup RowTable
		pRowTable = pCur;
		pCur += Info.nChannels * Info.Height * 2;

		FColor* Dest = (FColor*)OutData;

		// Loop through the planes
		for (iPlane = 0; iPlane < Info.nChannels; iPlane++)
		{
			int32 iWritePlane = iPlane;
			if (iWritePlane > BytesPerPixel - 1) iWritePlane = BytesPerPixel - 1;

			// Loop through the rows
			for (iRow = 0; iRow < Info.Height; iRow++)
			{
				// Load a row
				CompressedBytes = (pRowTable[(iPlane * Info.Height + iRow) * 2] << 8) +
					(pRowTable[(iPlane * Info.Height + iRow) * 2 + 1] << 0);

				// Setup Plane
				pPlane = pCur;
				pCur += CompressedBytes;

				// Decompress Row
				iPixel = 0;
				iByte = 0;
				while ((iPixel < Info.Width) && (iByte < CompressedBytes))
				{
					int8 code = static_cast<int8>(pPlane[iByte++]);

					// Is it a repeat?
					if (code < 0)
					{
						Count = -static_cast<int32>(code) + 1;
						Value = pPlane[iByte++];
						while (Count-- > 0)
						{
							int32 idx = (iPixel)+(iRow * Info.Width);
							if (Info.nChannels == 1)
							{
								Dest[idx].R = Value;
								Dest[idx].G = Value;
								Dest[idx].B = Value;
							}
							else
							{
								switch (iWritePlane)
								{
								case 0: Dest[idx].R = Value; break;
								case 1: Dest[idx].G = Value; break;
								case 2: Dest[idx].B = Value; break;
								case 3: Dest[idx].A = Value; break;
								default: break;
								}
							}
							iPixel++;
						}
					}
					// Must be a literal then
					else
					{
						Count = (int32)code + 1;
						while (Count-- > 0)
						{
							Value = pPlane[iByte++];
							int32 idx = (iPixel)+(iRow * Info.Width);

							if (Info.nChannels == 1)
							{
								Dest[idx].R = Value;
								Dest[idx].G = Value;
								Dest[idx].B = Value;
							}
							else
							{
								switch (iWritePlane)
								{
								case 0: Dest[idx].R = Value; break;
								case 1: Dest[idx].G = Value; break;
								case 2: Dest[idx].B = Value; break;
								case 3: Dest[idx].A = Value; break;
								default: break;
								}
							}
							iPixel++;
						}
					}
				}

				// Confirm that we decoded the right number of bytes
				check(iByte == CompressedBytes);
				check(iPixel == Info.Width);
			}
		}
	}
	else
		return false;

	// Success!
	return(true);
}

BYTANGS_END
