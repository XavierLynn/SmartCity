// Copyright 2020 byTangs, All Rights Reserved.


#include "RTIBlueprintFunctionLibrary.h"

#include "TextureImporter.h"
#include "Kismet/KismetStringLibrary.h"

UTexture2D* URTIBlueprintFunctionLibrary::ImportTexture(const FString& Filename)
{
    return  EMEngine::FTextureImporter::Get()->ImportTexture(Filename);
}

UTexture2D* URTIBlueprintFunctionLibrary::LoadTextureFormContentPath(UObject* Outer,const FString& TexturePath)
{
    if(TexturePath.IsEmpty())
        return nullptr;
 
    int32 FirstIndex = 0;
    FirstIndex = TexturePath.Find(TEXT("/"));

    const FString ObjectPath = UKismetStringLibrary::GetSubstring(TexturePath,FirstIndex,TexturePath.Len() - 1 - FirstIndex);
   
    return  LoadObject<UTexture2D>(Outer,*ObjectPath);
}
