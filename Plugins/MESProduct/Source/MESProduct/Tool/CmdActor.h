// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Misc/InteractiveProcess.h"
#include "CmdActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOutHandle,FString,text);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInputHandle, FString, text);

UCLASS()
class MESPRODUCT_API ACmdActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACmdActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MES组件属性", Meta = (DisplayName = "ExePath"))
		FString ExePath;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MES组件属性", Meta = (DisplayName = "ExeParam"))
		FString ExeParam;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MES组件属性", Meta = (DisplayName = "是否后台"))
		bool IsHidden = true;

	//启动程序
	UFUNCTION(BlueprintCallable, Category = "MESFunc")
		void Launch();

	UFUNCTION(BlueprintCallable, Category = "MESFunc")
		void InputCmd(FString text);

	UPROPERTY(BlueprintAssignable,Category = "MES组件属性")
		FOutHandle OutEvent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void* ReadPipeParent = nullptr;
	void* WritePipeChild = nullptr;
	void* ReadPipeChild = nullptr;
	void* WritePipeParent = nullptr;
	FProcHandle handle;

	FInteractiveProcess* proc;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
