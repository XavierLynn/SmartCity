// Copyright 2020 byTangs, All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "EMEngineCore.h"

//define log 
DECLARE_LOG_CATEGORY_EXTERN(RuntimeTextureImport,Log,All)


#define RTI_LOG(Verbosity,Format,...) \
        UE_LOG(RuntimeTextureImport,Verbosity,Format, ##__VA_ARGS__)