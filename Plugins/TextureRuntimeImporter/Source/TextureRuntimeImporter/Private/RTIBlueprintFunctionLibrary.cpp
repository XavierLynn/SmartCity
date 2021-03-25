// Copyright 2020 byTangs, All Rights Reserved.


#include "RTIBlueprintFunctionLibrary.h"

#include "TextureImporter.h"

UTexture2D* URTIBlueprintFunctionLibrary::ImportTexture(const FString& Filename)
{
    return  bytangs::FTextureImporter::Get()->ImportTexture(Filename);
}
