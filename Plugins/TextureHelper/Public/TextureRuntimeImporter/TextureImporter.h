// Copyright 2020 byTangs, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture.h"
#include "TextureRuntimeImporterCore.h"

/**
 * Texture Importer
 */

EME_START

class EMENGINE_API FTextureImporter
{
private:
	FTextureImporter() { }
	FTextureImporter(const FTextureImporter&) = delete;
	FTextureImporter& operator=(const FTextureImporter&) = delete;
public:

	~FTextureImporter();

	/** Get TextureImporter Singleton*/
	static FTextureImporter* Get()
	{
		if (!Singleton.IsValid())
		{
			Singleton = MakeShareable(new FTextureImporter());
		}
		return Singleton.Get();
	}

	static int32 GetBytesPerPixel(ETextureSourceFormat Format);
public:

	/** Import Texture from desk file path */
	UTexture2D* ImportTexture(const FString& Filename);

	UTexture2D* CreateUTexture2DByBuffer(TArray<uint8>& Buffer);

private:

	/** TextureImporter Singleton  */
	static TSharedPtr<FTextureImporter> Singleton;
};

EME_END