// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RabbitMqProtocol.h"

extern "C"
{
#include <amqp.h>
#include <amqp_tcp_socket.h>
#include <amqp_framing.h>
#include <amqp_ssl_socket.h>
}

#include "RabbitMqClient.generated.h"


#define RABBITMQ_MESSAGE_RESULT_NORMAL_STATUS 200

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessageReceiveMultiCallback, FString, JsonString);
DECLARE_DELEGATE_OneParam(FMessageReceiveCallback, FString)

DECLARE_DYNAMIC_DELEGATE_OneParam(FMqResponseCallback, FString, JsonString);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMQHeartBeatFailed);



USTRUCT(BlueprintType)
struct FRabbitMQConnectInfo
{ 
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Host = "10.0.8.30";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Port = 57080;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString VirtualHost = "/";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString UserName = "admin";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Password = "admin";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int TimeOut = 3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Channel = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Exchange = "me.chanpin_c04.exchange";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString ExchangeType = "topic";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString RoutingKey = "me.chanpin_c04.server";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FString> BindingKeys;

	UPROPERTY()
		FString RealExchange;
};


USTRUCT(BlueprintType)
struct FMessageReceiveResult
{
	GENERATED_BODY()

	UPROPERTY()
		int status;
	UPROPERTY()
		FString message;
	UPROPERTY()
		int type;
	UPROPERTY()
		int64 fromUuid;
};

USTRUCT(BlueprintType)
struct FMessageReceiveHandler
{
	GENERATED_BODY()

		//FMessageReceiveMultiCallback delegates;
		UPROPERTY()
		TMap<int, FMqResponseCallback> Handler;
};

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MESPRODUCT_API URabbitMqClient : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	URabbitMqClient();

	struct FMessageHeader
	{
		int ProtocolVersion = 0x00012081; // copy from ME
		int MessageType; // e.g. 10010002
		int ProtocolLength; // this.properties.ByteLength + Message.Length
		int SeqId = 0;
		int64 GUID; // suffix of bindingkey to consume

		FMessageHeader()
		{
		}

		void SetMsgLength(int length)
		{
			ProtocolLength = length;
		}
	};

public:
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(BlueprintAssignable, Category = "Components|HeartBeat")
		FOnMQHeartBeatFailed OnHeartBeatFailed;

	void UpdateSpecificExchange();

	UFUNCTION(BlueprintCallable)
		bool Connect();

	UFUNCTION(BlueprintCallable)
		void Disconnect();

	UFUNCTION(BlueprintCallable)
	void RequestWithCallback(int32 MsgID, const FString& MsgBody, FMqResponseCallback Callback);

	UFUNCTION(BlueprintCallable)
	void Request(const int32 MsgID,const FString& MsgBody)
	{
		MsgHeader.MessageType = MsgID;
		MsgHeader.SeqId++;

		if(MsgHeader.SeqId > 200000000)
			MsgHeader.SeqId = 0;

		BasicPublish(MsgBody);
	}

	void BasicPublish(const FString& MsgBody);

	UFUNCTION(BlueprintCallable)
		void LoginRequest(struct FMQLoginRequest Request, FMqResponseCallback Callback);

	UFUNCTION(BlueprintCallable)
		void LogOutRequest(struct FMQLogoutRequest Request);

	UFUNCTION(BlueprintCallable)
		void DataSourceListRequest(struct FMQDataSourceListRequest Request, FMqResponseCallback Callback);

	UFUNCTION(BlueprintCallable)
		void DataSourceColumnInfoRequest(struct FMQDataSourceColumnInfoRequest Request, FMqResponseCallback Callback);

	UFUNCTION(BlueprintCallable)
		void DataSourceColumnDataRequest(struct FMQDataSourceColumnDataRequest Request, FMqResponseCallback Callback);

	UFUNCTION(BlueprintCallable)
		static FString MakeMD5Str(FString UserName, FString Password);

	UFUNCTION(BlueprintCallable)
		void PageListRequest(struct FMQPageListRequest Request, FMqResponseCallback Callback);

	UFUNCTION(BlueprintCallable)
		void MenuListRequest(struct FMQMenuListRequest Request, FMqResponseCallback Callback);

	UFUNCTION(BlueprintCallable)
		void MenuUpdateRequest(struct FMQMenuUpdateRequest Request, FMqResponseCallback Callback);

	UFUNCTION(BlueprintCallable)
		void PageUpdateRequest(struct FMQPageUpdateRequest Request, FMqResponseCallback Callback);

	UFUNCTION(BlueprintCallable)
		void ComponentListRequest(struct FMQComponentListRequest Request, FMqResponseCallback Callback);

	UFUNCTION(BlueprintCallable)
		void DataSourceTableDataRequest(struct FMQDataSourceTableDataRequest Request, FMqResponseCallback Callback);

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool IsOpenSSL;*/

	void RegisterMessage(int MessageId, FMqResponseCallback Callback);
	void UnregisterMessage(int MessageId, UObject* Obj);

	UFUNCTION(BlueprintCallable)
		void UnregisterDataSourceColumnInfo(UObject* Obj);
	UFUNCTION(BlueprintCallable)
		void UnregisterDataSourceTableData(UObject* Obj);

	void AsyncReceive();
	void HandleRabbitAsyncData();
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FRabbitMQConnectInfo ConnectInfo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float HeartbeatInterval = 60.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float HeartbeatCheckInterval = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool IsShowPublishLog = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool IsShowReceiveLog = false;

private:
	bool HasReplyError(amqp_rpc_reply_t Reply, FString Context);
	bool HasResultError(int ResultCode, FString Context);

	bool	bQuit = false;

	struct FRabbitReceive
	{
		int32			SeqId;
		FString			TargetJsonString;
	};

	TQueue<FRabbitReceive> RabbitReceiveQueue;

	amqp_connection_state_t amqpConnection;
	amqp_socket_t* amqpSocket = nullptr;

	TArray<int> OpenedChannels;

	bool IsInitialized = false;

	FMessageHeader MsgHeader;
	char* MsgToPublish;
	unsigned char* bytes;

	int64 SelfGUID;

	UPROPERTY()
	TMap<int, FMessageReceiveHandler> MessageReceiveMap;// key1: message type, key2: Object Unique ID
	
	TMap<int32,FMqResponseCallback> MapResponse;

	FMqResponseCallback OnMqHeartbeatResponse;

	float HeartbeatTimer = 0.f;
	float HeartbeatCheckTimer = 0.f;
	bool bReceiveLastHeartbeatResponse = false;
	bool bSendHeartbeatRequest = false;
	 
	UFUNCTION()
		void OnHeartbeatResponse(const FString& JsonString);
};
