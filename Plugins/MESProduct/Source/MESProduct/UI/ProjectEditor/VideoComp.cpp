// Fill out your copyright notice in the Description page of Project Settings.


#include "VideoComp.h"

void UVideoComp::AsyncPlayVideo()
{
	auto task = new FAutoDeleteAsyncTask<VideoAutoDeleteAsyncTask>(this);

	if (task) task->StartBackgroundTask();
}