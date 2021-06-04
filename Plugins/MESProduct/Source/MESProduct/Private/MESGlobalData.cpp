// Fill out your copyright notice in the Description page of Project Settings.


#include "MESGlobalData.h"



bool UMESGlobalData::IsDesignMode = false;

void UMESGlobalData::SetDesignMode(bool IsDesign)
{
	UMESGlobalData::IsDesignMode = IsDesign;
}

bool UMESGlobalData::GetDesignMode()
{
	return UMESGlobalData::IsDesignMode;
}
