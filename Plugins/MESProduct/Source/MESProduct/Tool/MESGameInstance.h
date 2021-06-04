// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "RabbitMqClient.h"
#include "MESProduct/MESComponent/Http/HttpProtocol.h"
#include "MESProduct/UI/Launcher/LauncherSaveGame.h"
#include "MESGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class MESPRODUCT_API UMESGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString UserName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FProjectInfo ProjectInfo_Offline;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FHttpProjectData ProjectInfo_Server;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FRabbitMQConnectInfo MqConnectInfo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool IsProjectOffline;

	/*UFUNCTION(BlueprintCallable, Category = "MESFunc", meta = (WorldContext = "WorldContextObject"))
		static UMESGameInstance* GetMESGameInstance(const UObject* WorldContextObject);*/
	static UMESGameInstance* Get(const UWorld* World);
};
