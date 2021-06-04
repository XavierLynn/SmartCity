// Fill out your copyright notice in the Description page of Project Settings.


#include "MESPathUtility.h"
#include "Misc/Paths.h"

FString UMESPathUtility::GetProductBasePath()
{
	return FPaths::ProjectDir() + "\\StreamingAssets\\MESProduct\\";
}

FString UMESPathUtility::GetProjectBasePath()
{
	return FPaths::ProjectDir() + "\\StreamingAssets\\MESProject\\";
}

FString UMESPathUtility::GetStandaloneProjectDBPath(FString ProjectName)
{
	return GetProjectBasePath() + TEXT("单机\\") + ProjectName + TEXT("\\数据\\配置数据库.db");
}

FString UMESPathUtility::GetStandaloneProjectBaseDBPath()
{
	return GetProductBasePath() + TEXT("数据\\配置数据库.db");
}

FString UMESPathUtility::GetProductDataPath()
{
	return GetProductBasePath() + TEXT("数据\\");
}

FString UMESPathUtility::GetStandaloneProjectDataPath(FString ProjectName)
{
	return GetProjectBasePath() + TEXT("单机\\") + ProjectName + TEXT("\\数据\\");
}

FString UMESPathUtility::GetScreenShotPath()
{
	return GetProjectBasePath() + TEXT("ScreenShot\\") ;
}

FString UMESPathUtility::GetStandaloneProjectResourcePath(FString ProjectName)
{
	return GetProjectBasePath() + TEXT("单机\\") + ProjectName + TEXT("\\资源\\");
}

FString UMESPathUtility::GetServerProjectDataPath(FString ProjectName)
{
	return GetProjectBasePath() + TEXT("联网\\") + ProjectName + TEXT("\\数据\\");
}

FString UMESPathUtility::GetServerProjectDBPath(FString ProjectName)
{
	return GetProjectBasePath() + TEXT("联网\\") + ProjectName + TEXT("\\数据\\配置数据库.db");
}
