// Fill out your copyright notice in the Description page of Project Settings.


#include "CmdActor.h"
#include "Misc/InteractiveProcess.h"

// Sets default values
ACmdActor::ACmdActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

void ACmdActor::Launch()
{
	proc = new FInteractiveProcess(ExePath, ExeParam, IsHidden, true);
	proc->OnOutput().BindLambda([=](FString tex) {
		if (OutEvent.IsBound())
		{
			OutEvent.Broadcast(tex);
		}
	});
	proc->Launch();
}

void ACmdActor::InputCmd(FString text)
{
	if (proc != nullptr)
	{
		proc->SendWhenReady(text);
	}
}

// Called when the game starts or when spawned
void ACmdActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACmdActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

