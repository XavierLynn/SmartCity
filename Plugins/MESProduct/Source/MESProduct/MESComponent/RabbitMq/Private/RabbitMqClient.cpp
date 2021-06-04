// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "RabbitMqClient.h"

#include <functional>

#include "WinSock2.h"
#include <sstream>
#include <thread>

#include "Kismet/KismetSystemLibrary.h"
#include "Serialization\BufferARchive.h"
#include "Misc/SecureHash.h"

URabbitMqClient::URabbitMqClient()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	//IsOpenSSL = true;
}

void URabbitMqClient::UpdateSpecificExchange()
{
	ConnectInfo.RoutingKey = "me." + ConnectInfo.Exchange + ".server";
	ConnectInfo.RealExchange = "me." + ConnectInfo.Exchange + ".exchange";
}

bool URabbitMqClient::Connect()
{
	amqpConnection = amqp_new_connection();

	/*if (IsOpenSSL)
	{
		amqpSocket = amqp_ssl_socket_new(amqpConnection);
	}
	else
	{*/
	amqpSocket = amqp_tcp_socket_new(amqpConnection);
	//}
	if (!amqpSocket)
	{
		UE_LOG(LogTemp, Log, TEXT("amqp_tcp_socket_new failed!"));
		return false;
	}

	/*if (IsOpenSSL)
	{
		Port -= 1;

		amqp_ssl_socket_set_verify_peer(amqpSocket, 0);
		amqp_ssl_socket_set_verify_hostname(amqpSocket, 0);

		FString CacertFullPath("mq_cert.pem");
		HasResultError(amqp_ssl_socket_set_cacert(amqpSocket, TCHAR_TO_ANSI(*CacertFullPath)),
			"amqp_ssl_socket_set_cacert");
	}*/

	timeval timeOut;
	timeOut.tv_sec = ConnectInfo.TimeOut;
	timeOut.tv_usec = 0;
	int socketStatus = amqp_socket_open_noblock(amqpSocket, TCHAR_TO_ANSI(*ConnectInfo.Host), ConnectInfo.Port, &timeOut);

	//int socketStatus = amqp_socket_open(amqpSocket, TCHAR_TO_ANSI(*Host), Port);

	if (socketStatus != AMQP_STATUS_OK)
	{
		UE_LOG(LogTemp, Log, TEXT("amqp_socket_open failed, error status: %d"), socketStatus);
		return false;
	}

	if (HasReplyError(
		amqp_login(amqpConnection, TCHAR_TO_ANSI(*ConnectInfo.VirtualHost), AMQP_DEFAULT_MAX_CHANNELS, AMQP_DEFAULT_FRAME_SIZE,
			0, AMQP_SASL_METHOD_PLAIN, TCHAR_TO_ANSI(*ConnectInfo.UserName), TCHAR_TO_ANSI(*ConnectInfo.Password)),
		"amqp_login"))
	{
		return false;
	}

	if (OpenedChannels.Contains(ConnectInfo.Channel))
	{
		UE_LOG(LogTemp, Log, TEXT("ChannelInitialize failed, this channel has been opened: %d"), ConnectInfo.Channel);
		return false;
	}

	amqp_channel_open(amqpConnection, ConnectInfo.Channel);
	if (HasReplyError(amqp_get_rpc_reply(amqpConnection), "amqp_channel_open"))
	{
		return false;
	}

	const FString TargetExchange = ConnectInfo.Exchange;
	UpdateSpecificExchange();
	{
		amqp_exchange_declare(amqpConnection, ConnectInfo.Channel, amqp_cstring_bytes(TCHAR_TO_ANSI(*ConnectInfo.RealExchange)),
			amqp_cstring_bytes(TCHAR_TO_ANSI(*ConnectInfo.ExchangeType)), 0, 0, 1, 0, amqp_empty_table);

		if (HasReplyError(amqp_get_rpc_reply(amqpConnection), "amqp_exchange_declare"))
		{
			return false;
		}
	}

	OpenedChannels.Add(ConnectInfo.Channel);

	/*if (!OpenedChannels.Contains(ConnectInfo.Channel))
	{
		UE_LOG(LogTemp, Log, TEXT("Consume failed, this channel has not been opened"));
		return;
	}*/

	amqp_bytes_t queuename;
	{
		amqp_queue_declare_ok_t* r = amqp_queue_declare(
			amqpConnection, ConnectInfo.Channel, amqp_empty_bytes, 0, 0, 0, 1, amqp_empty_table);
		if (HasReplyError(amqp_get_rpc_reply(amqpConnection), "amqp_queue_declare") || r == NULL)
		{
			return false;
		}
		queuename = amqp_bytes_malloc_dup(r->queue);
		if (queuename.bytes == NULL) {
			UE_LOG(LogTemp, Log, TEXT("amqp_bytes_malloc_dup error while copying queue name"));
			return false;
		}
	}

	//binding guid key.
	TArray<FString> BindingKeys;
	std::stringstream GUIDBuff;
	GUIDBuff << "me." << TCHAR_TO_ANSI(*TargetExchange) << "." << MsgHeader.GUID;
	BindingKeys.Add(GUIDBuff.str().c_str());

	FString BindingClient = "me." + TargetExchange + ".pc.client";
	FString BindingEditor = "me." + TargetExchange + ".pc.editor";
	FString BindingRunner = "me." + TargetExchange + ".pc.runner";
	BindingKeys.Add(BindingClient);
	BindingKeys.Add(BindingEditor);
	BindingKeys.Add(BindingRunner);
	BindingKeys.Append(ConnectInfo.BindingKeys);

	bool HasBindingKeyError = false;
	for (int i = 0; i < BindingKeys.Num(); ++i)
	{
		FString TargetBindingKey = BindingKeys[i];
		amqp_queue_bind(amqpConnection, ConnectInfo.Channel, queuename, amqp_cstring_bytes(TCHAR_TO_ANSI(*ConnectInfo.RealExchange)),
			amqp_cstring_bytes(TCHAR_TO_ANSI(*TargetBindingKey)), amqp_empty_table);
		if (HasReplyError(amqp_get_rpc_reply(amqpConnection), "amqp_queue_bind"))
		{
			HasBindingKeyError = true;
		}
	}

	amqp_basic_consume(amqpConnection, ConnectInfo.Channel, queuename, amqp_empty_bytes, 0, 1, 0, amqp_empty_table);
	HasReplyError(amqp_get_rpc_reply(amqpConnection), "amqp_basic_consume");

	IsInitialized = true;
	return !HasBindingKeyError;
}

void URabbitMqClient::Disconnect()
{
	if (IsInitialized)
	{
		for (int channelIndex = 0; channelIndex < OpenedChannels.Num(); ++channelIndex)
		{
			int openedChannel = OpenedChannels[channelIndex];
			FString Context("amqp_channel_close ");
			Context.Append(FString::FromInt(openedChannel));
			HasReplyError(amqp_channel_close(amqpConnection, openedChannel, AMQP_REPLY_SUCCESS), Context);
		}
		OpenedChannels.Empty();

		HasReplyError(amqp_connection_close(amqpConnection, AMQP_REPLY_SUCCESS),
			"amqp_connection_close");
		HasResultError(amqp_destroy_connection(amqpConnection), "amqp_destroy_connection");
		IsInitialized = false;
	}

	/*if (IsOpenSSL)
	{
		HasResultError(amqp_uninitialize_ssl_library(), "amqp_uninitialize_ssl_library");
	}*/
}

void URabbitMqClient::RequestWithCallback(int32 MsgID, const FString& MsgBody, FMqResponseCallback Callback)
{
	MsgHeader.MessageType = MsgID;
	MsgHeader.SeqId++;

	if(MsgHeader.SeqId > 200000000)
		MsgHeader.SeqId = 1;
	
	if (Callback.IsBound())
		MapResponse.Add(MsgHeader.SeqId,Callback);
	
	BasicPublish(MsgBody);
}

void URabbitMqClient::BasicPublish(const FString& MsgBodyO)
{
	if (!IsInitialized)
		return;

	FString MsgBody = MsgBodyO;
	if (MsgBody.IsEmpty())
		MsgBody = "{}";

	amqp_basic_properties_t props;
	props._flags = /*AMQP_BASIC_CONTENT_TYPE_FLAG |*/ AMQP_BASIC_DELIVERY_MODE_FLAG;
	//props.content_type = amqp_cstring_bytes("text/plain");
	props.delivery_mode = 2; /* persistent delivery mode */

	TArray<uint8> BodyBinary;
	FTCHARToUTF8 Converter(*MsgBody);
	BodyBinary.SetNum(Converter.Length());
	FMemory::Memcpy(BodyBinary.GetData(), (uint8*)(ANSICHAR*)Converter.Get(), BodyBinary.Num());

	MsgHeader.SetMsgLength(sizeof(int) * 4 + sizeof(int64) + BodyBinary.Num());

	TArray<uint8> header;
	header.Add(MsgHeader.ProtocolVersion >> 24);
	header.Add(MsgHeader.ProtocolVersion >> 16);
	header.Add(MsgHeader.ProtocolVersion >> 8);
	header.Add(MsgHeader.ProtocolVersion);

	header.Add(MsgHeader.MessageType >> 24);
	header.Add(MsgHeader.MessageType >> 16);
	header.Add(MsgHeader.MessageType >> 8);
	header.Add(MsgHeader.MessageType);

	header.Add(MsgHeader.ProtocolLength >> 24);
	header.Add(MsgHeader.ProtocolLength >> 16);
	header.Add(MsgHeader.ProtocolLength >> 8);
	header.Add(MsgHeader.ProtocolLength);

	header.Add(MsgHeader.SeqId >> 24);
	header.Add(MsgHeader.SeqId >> 16);
	header.Add(MsgHeader.SeqId >> 8);
	header.Add(MsgHeader.SeqId);

	header.Add(MsgHeader.GUID >> 56);
	header.Add(MsgHeader.GUID >> 48);
	header.Add(MsgHeader.GUID >> 40);
	header.Add(MsgHeader.GUID >> 32);
	header.Add(MsgHeader.GUID >> 24);
	header.Add(MsgHeader.GUID >> 16);
	header.Add(MsgHeader.GUID >> 8);
	header.Add(MsgHeader.GUID);

	FBufferArchive ToBinary;
	ToBinary.Append(header);
	ToBinary.Append(BodyBinary);

	uint8 * TargetData = ToBinary.GetData();
	amqp_bytes_t_ target;
	target.len = ToBinary.Num();
	target.bytes = (void*)TargetData;

	HasResultError(amqp_basic_publish(amqpConnection, ConnectInfo.Channel, amqp_cstring_bytes(TCHAR_TO_ANSI(*ConnectInfo.RealExchange)),
        amqp_cstring_bytes(TCHAR_TO_ANSI(*ConnectInfo.RoutingKey)), 0, 0,
        &props, target),
        "amqp_basic_publish");

	ToBinary.FlushCache();
	ToBinary.Empty();
}

void URabbitMqClient::LoginRequest(FMQLoginRequest Request, FMqResponseCallback Callback)
{
	FString Credentials = MakeMD5Str(Request.UserName, Request.Password);
	MsgHeader.MessageType = RABBITMQ_LOGIN_REQUEST;
	//RegisterMessage(RABBITMQ_LOGIN_RESPONSE, Callback);
	//BasicPublish(Request.ToString(Credentials));
	RequestWithCallback(RABBITMQ_LOGIN_REQUEST,Request.ToString(Credentials),Callback);
}

void URabbitMqClient::LogOutRequest(FMQLogoutRequest Request)
{
	//MsgHeader.MessageType = RABBITMQ_LOGOUT_REQUEST;
	this->Request(RABBITMQ_LOGOUT_REQUEST,Request.ToString());
}

void URabbitMqClient::DataSourceListRequest(FMQDataSourceListRequest Request, FMqResponseCallback Callback)
{
	//MsgHeader.MessageType = RABBITMQ_DATA_SOURCE_LIST_REQUEST;
	//RegisterMessage(RABBITMQ_DATA_SOURCE_LIST_RESPONSE, Callback);
	RequestWithCallback(RABBITMQ_DATA_SOURCE_LIST_REQUEST, Request.ToString(),Callback);
}

void URabbitMqClient::DataSourceColumnInfoRequest(FMQDataSourceColumnInfoRequest Request, FMqResponseCallback Callback)
{
	//MsgHeader.MessageType = RABBITMQ_DATA_SOURCE_COLUMN_INFO_REQUEST;
	//RegisterMessage(RABBITMQ_DATA_SOURCE_COLUMN_INFO_RESPONSE, Callback);
	RequestWithCallback(RABBITMQ_DATA_SOURCE_COLUMN_INFO_REQUEST,Request.ToString(),Callback);
}

void URabbitMqClient::DataSourceColumnDataRequest(FMQDataSourceColumnDataRequest Request, FMqResponseCallback Callback)
{
	//MsgHeader.MessageType = RABBITMQ_DATA_SOURCE_COLUMU_DATA_REQUEST;
	//RegisterMessage(RABBITMQ_DATA_SOURCE_COLUMU_DATA_RESPONSE, Callback);
	RequestWithCallback(RABBITMQ_DATA_SOURCE_COLUMU_DATA_REQUEST,Request.ToString(),Callback);
}

FString URabbitMqClient::MakeMD5Str(FString UserName, FString Password)
{
	FString MD5Str = Password + "{" + UserName + "}";
	FString MD5Result = FMD5::HashAnsiString(*MD5Str);
	return MD5Result;
}

void URabbitMqClient::PageListRequest(FMQPageListRequest Request, FMqResponseCallback Callback)
{
	//MsgHeader.MessageType = RABBITMQ_PAGE_LIST_REQUEST;
	//RegisterMessage(RABBITMQ_PAGE_LIST_RESPONSE, Callback);
	RequestWithCallback(RABBITMQ_PAGE_LIST_REQUEST, Request.ToString(),Callback);
}

void URabbitMqClient::MenuListRequest(FMQMenuListRequest Request, FMqResponseCallback Callback)
{
	//MsgHeader.MessageType = RABBITMQ_MENU_LIST_REQUEST;
	//RegisterMessage(RABBITMQ_MENU_LIST_RESPONSE, Callback);
	RequestWithCallback(RABBITMQ_MENU_LIST_REQUEST,Request.ToString(),Callback);
}

void URabbitMqClient::MenuUpdateRequest(FMQMenuUpdateRequest Request, FMqResponseCallback Callback)
{
	//MsgHeader.MessageType = RABBITMQ_MENU_UPDATE_REQUEST;
	//RegisterMessage(RABBITMQ_MENU_UPDATE_RESPONSE, Callback);
	RequestWithCallback(RABBITMQ_MENU_UPDATE_REQUEST,Request.ToString(),Callback);
}

void URabbitMqClient::PageUpdateRequest(FMQPageUpdateRequest Request, FMqResponseCallback Callback)
{
	//MsgHeader.MessageType = RABBITMQ_PAGE_UPDATE_REQUEST;
	//RegisterMessage(RABBITMQ_PAGE_UPDATE_RESPONSE, Callback);
	//RegisterMessage(RABBITMQ_COMPONENT_UPDATE_RESPONSE, Callback);
	RequestWithCallback(RABBITMQ_PAGE_UPDATE_REQUEST,Request.ToString(),Callback);
}

void URabbitMqClient::ComponentListRequest(FMQComponentListRequest Request, FMqResponseCallback Callback)
{
	//MsgHeader.MessageType = RABBITMQ_COMPONENT_LIST_REQUEST;
	//RegisterMessage(RABBITMQ_COMPONENT_LIST_RESPONSE, Callback);
	RequestWithCallback(RABBITMQ_COMPONENT_LIST_REQUEST,Request.ToString(),Callback);
}

void URabbitMqClient::DataSourceTableDataRequest(FMQDataSourceTableDataRequest Request, FMqResponseCallback Callback)
{
	//MsgHeader.MessageType = RABBITMQ_DATA_SOURCE_TABLE_DATA_REQUEST;
	//RegisterMessage(RABBITMQ_DATA_SOURCE_TABLE_DATA_RESPONSE, Callback);
	RequestWithCallback(RABBITMQ_DATA_SOURCE_TABLE_DATA_REQUEST,Request.ToString(),Callback);
}

void URabbitMqClient::RegisterMessage(int MessageId, FMqResponseCallback Callback)
{
	if (!Callback.GetUObject())
	{
		UE_LOG(LogTemp, Warning, TEXT("RabbitMq RegisterMessage error, Obj of Callback is null"));
		return;
	}

	int UniqueId = Callback.GetUObject()->GetUniqueID();
	if (!MessageReceiveMap.Contains(MessageId))
	{
		/*TArray<FMqResponseCallback> Callbacks;
		Callbacks.Add(Callback);
		MessageReceiveMap.Add(MessageId, Callbacks);*/

		/*FMessageReceiveMultiCallback MultiCallback;
		MultiCallback.Add(Callback);
		MessageReceiveMap.Add(MessageId, MultiCallback);*/

		FMessageReceiveHandler NewHandler;
		NewHandler.Handler.Add(UniqueId, Callback);
		MessageReceiveMap.Add(MessageId, NewHandler);
	}
	else
	{
		if (!MessageReceiveMap[MessageId].Handler.Contains(UniqueId))
		{
			MessageReceiveMap[MessageId].Handler.Add(UniqueId, Callback);
		}
		else
		{
			MessageReceiveMap[MessageId].Handler[UniqueId] = Callback;
		}
	}
}

void URabbitMqClient::UnregisterMessage(int MessageId, UObject* Obj)
{
	if (!Obj)
	{
		UE_LOG(LogTemp, Warning, TEXT("RabbitMq UnregisterMessage error, Obj is null"));
		return;
	}
	int UniqueId = Obj->GetUniqueID();
	if (MessageReceiveMap.Contains(MessageId) && MessageReceiveMap[MessageId].Handler.Contains(UniqueId))
	{
		MessageReceiveMap[MessageId].Handler.Remove(UniqueId);
	}
}

void URabbitMqClient::UnregisterDataSourceColumnInfo(UObject * Obj)
{
	UnregisterMessage(RABBITMQ_DATA_SOURCE_COLUMN_INFO_RESPONSE, Obj);
}

void URabbitMqClient::UnregisterDataSourceTableData(UObject * Obj)
{
	UnregisterMessage(RABBITMQ_DATA_SOURCE_TABLE_DATA_RESPONSE, Obj);
}

bool URabbitMqClient::HasReplyError(amqp_rpc_reply_t Reply, FString Context)
{
	switch (Reply.reply_type) {
	case AMQP_RESPONSE_NORMAL:
		return false;

	case AMQP_RESPONSE_NONE:
		UE_LOG(LogTemp, Log, TEXT("%s: missing RPC reply type!\n"), *Context);
		break;

	case AMQP_RESPONSE_LIBRARY_EXCEPTION:
		UE_LOG(LogTemp, Log, TEXT("%s: %s\n"), *Context, ANSI_TO_TCHAR(amqp_error_string2(Reply.library_error)));
		break;

	case AMQP_RESPONSE_SERVER_EXCEPTION:
		switch (Reply.reply.id) {
		case AMQP_CONNECTION_CLOSE_METHOD: {
			amqp_connection_close_t* m = (amqp_connection_close_t*)Reply.reply.decoded;
			UE_LOG(LogTemp, Log, TEXT("%s: server connection error %u, message: %s\n"),
				*Context, m->reply_code, ANSI_TO_TCHAR((char*)m->reply_text.bytes));
			break;
		}
		case AMQP_CHANNEL_CLOSE_METHOD: {
			amqp_channel_close_t* m = (amqp_channel_close_t*)Reply.reply.decoded;
			UE_LOG(LogTemp, Log, TEXT("%s: server channel error %u, message: %s\n"),
				*Context, m->reply_code, ANSI_TO_TCHAR((char*)m->reply_text.bytes));
			break;
		}
		default:
			UE_LOG(LogTemp, Log, TEXT("%s: unknown server error, method id 0x%08X\n"),
				*Context, Reply.reply.id);
			break;
		}
		break;
	}
	return true;
}

bool URabbitMqClient::HasResultError(int ResultCode, FString Context)
{
	if (ResultCode < 0) {
		UE_LOG(LogTemp, Log, TEXT("%s result error: %s\n"), *Context, ANSI_TO_TCHAR(amqp_error_string2(ResultCode)));
		return true;
	}
	//UE_LOG(LogTemp, Log, TEXT("%s ing"), *Context);
	return false;
}

void URabbitMqClient::AsyncReceive()
{
	while (true)
	{
		if (UKismetSystemLibrary::IsValid(this) == false)
			break;
		
		if (bQuit)
			break;

		if (IsInitialized)
		{
			amqp_rpc_reply_t res;
			amqp_envelope_t envelope;

			amqp_maybe_release_buffers(amqpConnection);

			timeval consumeTime;
			consumeTime.tv_sec = 0;
			consumeTime.tv_usec = 0;
			res = amqp_consume_message(amqpConnection, &envelope, &consumeTime, 0);
			//amqp_simple_wait_frame(amqpConnection, )
			amqp_boolean_t result = amqp_frames_enqueued(amqpConnection);

			if (AMQP_RESPONSE_NORMAL == res.reply_type)
			{
				unsigned char* MsgReceieve = reinterpret_cast<unsigned char*>(envelope.message.body.bytes);

				if (envelope.message.body.len >= 24)
				{
					URabbitMqClient::FMessageHeader ReceiveHeader;
					memcpy(&ReceiveHeader.ProtocolVersion, MsgReceieve, sizeof(int));
					memcpy(&ReceiveHeader.MessageType, MsgReceieve + sizeof(int), sizeof(int));
					memcpy(&ReceiveHeader.ProtocolLength, MsgReceieve + sizeof(int) * 2, sizeof(int));
					memcpy(&ReceiveHeader.SeqId, MsgReceieve + sizeof(int) * 3, sizeof(int));
					memcpy(&ReceiveHeader.GUID, MsgReceieve + sizeof(int) * 4, sizeof(int64));

					ReceiveHeader.ProtocolVersion = ntohl(ReceiveHeader.ProtocolVersion);
					ReceiveHeader.MessageType = ntohl(ReceiveHeader.MessageType);
					ReceiveHeader.ProtocolLength = ntohl(ReceiveHeader.ProtocolLength);
					ReceiveHeader.SeqId = ntohl(ReceiveHeader.SeqId);
					ReceiveHeader.GUID = ntohl(ReceiveHeader.GUID);

					if (ReceiveHeader.ProtocolLength >= 24)
					{
						int MsgBodyLen = envelope.message.body.len - (sizeof(int) * 4 + sizeof(int64));

						TArray<uint8> BodyMsg;
						BodyMsg.SetNum(MsgBodyLen);
						memcpy(BodyMsg.GetData(), MsgReceieve + sizeof(int) * 4 + sizeof(int64), MsgBodyLen);
						BodyMsg.Add('\0');

						FString TargetJsonString(UTF8_TO_TCHAR(BodyMsg.GetData()));

						/*FRabbitReceive RabbitReceive;
						RabbitReceive.SeqId = ReceiveHeader.SeqId;
						int 
						RabbitReceive.TargetJsonString = TargetJsonString;

						RabbitReceiveQueue.Enqueue(RabbitReceive);*/

						int32 SeqId = ReceiveHeader.SeqId;

						AsyncTask(ENamedThreads::GameThread, [this,SeqId,TargetJsonString]()
					    {
							FMqResponseCallback Callback;
							
							//UE_LOG(LogTemp, Warning, TEXT("rabbitmq receive result json seqid=%d, string = %s"), SeqId,*TargetJsonString);

							if (MapResponse.RemoveAndCopyValue(SeqId,Callback))
							{
							    if (Callback.IsBound())
							    {
							        Callback.Execute(TargetJsonString);
							        Callback.Unbind();
							    }
							}
					    });
					}
				}
			}
			amqp_destroy_envelope(&envelope);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}
}

void URabbitMqClient::HandleRabbitAsyncData()
{
	if (RabbitReceiveQueue.IsEmpty() == false)
	{
		FRabbitReceive RabbitReceive;
		if (RabbitReceiveQueue.Dequeue(RabbitReceive))
		{
			FMqResponseCallback Callback;

			if (MapResponse.RemoveAndCopyValue(RabbitReceive.SeqId,Callback))
			{
				if (Callback.IsBound())
				{
					Callback.Execute(RabbitReceive.TargetJsonString);
					Callback.Unbind();
				}
			}
		}
	}
}

void URabbitMqClient::OnHeartbeatResponse(const FString & JsonString)
{
	UE_LOG(LogTemp, Warning, TEXT("rabbitmq heartbeat response json string: %s"), *JsonString);
	bReceiveLastHeartbeatResponse = true;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString(" Received HeartBeat..."));
}

void URabbitMqClient::BeginPlay()
{
	Super::BeginPlay();

	FGuid uuid = FGuid::NewGuid();
	SelfGUID = uuid.IsValid() ? uuid[0] + uuid[1] : 0;
	SelfGUID = FMath::Abs(SelfGUID);
	//SelfGUID = 1;
	//UE_LOG(LogTemp, Log, TEXT("guid : %s"), *FString::FromInt(SelfGUID));
	MsgHeader.GUID = SelfGUID;

	OnMqHeartbeatResponse.BindUFunction(this, "OnHeartbeatResponse");
	//RegisterMessage(RABBITMQ_LOGIN_RESPONSE, this, &URabbitMqClient::OnReceiveLogin);

	/*std::thread Thread(AsyncReceive, this);
	Thread.detach();*/

	std::thread(std::bind(&URabbitMqClient::AsyncReceive, this)).detach();
}

void URabbitMqClient::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsPendingKillOrUnreachable() || !IsValidLowLevelFast())
	{
		return;
	}

	if (!IsInitialized)
	{
		return;
	}

	//Receive();
	//HandleRabbitAsyncData();

	HeartbeatTimer += DeltaTime;
	if (HeartbeatTimer >= HeartbeatInterval)
	{
		HeartbeatTimer = 0.f;
		MsgHeader.MessageType = RABBITMQ_HEARTBEAT_REQUEST;
		//RegisterMessage(RABBITMQ_HEARTBEAT_RESPONSE, OnMqHeartbeatResponse);
		//BasicPublish("{}");
		RequestWithCallback(RABBITMQ_HEARTBEAT_REQUEST,"{}",OnMqHeartbeatResponse);
		bSendHeartbeatRequest = true;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString(" Send HeartBeat..."));
	}

	if (bSendHeartbeatRequest)
	{
		HeartbeatCheckTimer += DeltaTime;
		if (HeartbeatCheckTimer >= HeartbeatCheckInterval)
		{
			if (!bReceiveLastHeartbeatResponse)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString(" Receive HeartBeat Failed..."));

				OnHeartBeatFailed.Broadcast();
				HeartbeatTimer = 0.f;
			}
			else
			{
				bReceiveLastHeartbeatResponse = false;
			}
			bSendHeartbeatRequest = false;
			HeartbeatCheckTimer = 0.f;
		}
	}
}

void URabbitMqClient::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	bQuit = true;
	
	IsInitialized = false;

	MapResponse.Empty();

	Super::EndPlay(EndPlayReason);
}
