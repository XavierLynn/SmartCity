// Copyright 2020 byTangs, All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture.h"
#include "TextureRuntimeImporterCore.h"

EME_START

struct FIBITMAP;
struct FIMEMORY;

class FTifTextureHelper
{
public:

	FTifTextureHelper();

	~FTifTextureHelper();

	bool Load(const uint8 * Buffer, uint32 Length);

	bool ConvertToRGBA16();

	void SetError(const FString& InErrorMessage);

	FString GetError();

	bool IsValid();

	// Resulting image data and properties
	TArray<uint8> RawData;
	int32 Width;
	int32 Height;
	ETextureSourceFormat TextureSourceFormat = TSF_Invalid;
	TextureCompressionSettings CompressionSettings = TC_Default;
	bool bSRGB = true;

private:
	bool bIsValid = false;
	FIBITMAP* Bitmap = nullptr;
	FIMEMORY* Memory = nullptr;

	FString ErrorMessage;
};

EME_END