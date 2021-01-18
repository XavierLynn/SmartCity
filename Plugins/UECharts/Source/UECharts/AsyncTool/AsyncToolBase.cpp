// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncToolBase.h"
#include "AsyncManagerActor.h"


#include "Modules/ModuleManager.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"


void UAsyncToolBase::StopUpdate()
{
	SetReadyToDestroy();
}

void UAsyncToolBase::Init(float durTime)
{
	FTickHandle updateCall = FTickHandle();
	updateCall.BindUObject(this, &UAsyncToolBase::UpdateFunc);
	TotalTime = durTime;
	UpdateKey = AAsyncManagerActor::RegisterUpdateCall(updateCall);
}
void UAsyncToolBase::SetReadyToDestroy()
{
	if (Finished)return;
	Finished = true;
	AAsyncManagerActor::RemoveUpdateCall(UpdateKey);
	Super::SetReadyToDestroy();
	//auto a = [_path, _key, ImageAsync, ImageWrapper]() {
	//	UTexture2D* Texture = ImageAsync->LoadTexture2D(_path, _key, ImageWrapper);
	//	FOnImageLoadComplete OnComplete = ImageAsync->ImageLoadComplete;
	//	OnComplete.Broadcast(Texture, _key);
	//};
	////TSharedPtr<IImageWrapper> ImageWrapper = ImageAsync->GetImageWrapperByExtention(".jpeg");
	//Async<void>(EAsyncExecution::ThreadPool, a);

}
void UAsyncToolBase::UpdateFunc(float deltaTime)
{
	if (Finished)return;
	
	CurTime += deltaTime;
	if (CurTime >= TotalTime)//Finish
	{
		SetReadyToDestroy();
	}
	

}


