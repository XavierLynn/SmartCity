
#include "RabbitMqProtocol.h"
#include "JsonUtilities.h"


#define MES_MQ_RESPONSE_DEFINE(PROTOCOL_NAME) \
FMQ##PROTOCOL_NAME##Response URabbitMqProtocol::ParseMQ##PROTOCOL_NAME##Response(FString JsonString) \
{ \
	FMQ##PROTOCOL_NAME##Response Result; \
	const FString OriginJsonString = JsonString;\
	while (!FJsonObjectConverter::JsonObjectStringToUStruct<FMQ##PROTOCOL_NAME##Response>(JsonString, &Result, 0, 0))\
	{\
		if (JsonString.Len() > 0)\
		{\
			JsonString = JsonString.Left(JsonString.Len() - 1);\
		}\
		else\
		{\
			UE_LOG(LogTemp, Error, TEXT("Parse Json Error! JsonString: %s"), *OriginJsonString);\
		}\
		int JsonEndSymbolIndex;\
		if (!JsonString.FindLastChar('}', JsonEndSymbolIndex))\
		{\
			UE_LOG(LogTemp, Error, TEXT("Parse Json Error! JsonString: %s"), *OriginJsonString);\
			break;\
		}\
		JsonString = JsonString.Mid(0, JsonEndSymbolIndex + 1);\
	}\
	return Result; \
}

#define MES_MQ_RESPONSE_INIT_DEFINE(PROTOCOL_NAME) \
FMQ##PROTOCOL_NAME##Response URabbitMqProtocol::ParseMQ##PROTOCOL_NAME##Response(FString JsonString) \
{ \
	FMQ##PROTOCOL_NAME##Response Result; \
	Result.Init(JsonString);\
	return Result; \
}

MES_MQ_RESPONSE_DEFINE(Login)
MES_MQ_RESPONSE_DEFINE(DataSourceList)
MES_MQ_RESPONSE_DEFINE(DataSourceColumnInfo)
MES_MQ_RESPONSE_DEFINE(DataSourceColumnData)
//MES_MQ_RESPONSE_DEFINE(PageList)
//MES_MQ_RESPONSE_DEFINE(MenuList)
MES_MQ_RESPONSE_DEFINE(PageUpdate)
MES_MQ_RESPONSE_DEFINE(MenuUpdate)
MES_MQ_RESPONSE_DEFINE(ComponentUpdate)
//MES_MQ_RESPONSE_DEFINE(ComponentList)
//MES_MQ_RESPONSE_DEFINE(DataSourceTableData)

MES_MQ_RESPONSE_INIT_DEFINE(PageList)
MES_MQ_RESPONSE_INIT_DEFINE(MenuList)
MES_MQ_RESPONSE_INIT_DEFINE(ComponentList)
MES_MQ_RESPONSE_INIT_DEFINE(DataSourceTableData)

TArray<FMQMenuData> URabbitMqProtocol::GetMQChildMenuData(FMQMenuData ParentMenu)
{
	return ParentMenu.ChildMenuList;
}

//FMQPageListResponse URabbitMqProtocol::ParseMQPageListResponse(FString JsonString)
//{
//	FMQPageListResponse Result;
//	const FString OriginJsonString = JsonString;
//	while (!FJsonObjectConverter::JsonObjectStringToUStruct<FMQPageListResponse>(JsonString, &Result, 0, 0))
//	{
//		if (JsonString.Len() > 0)
//		{
//			JsonString = JsonString.Left(JsonString.Len() - 1);
//		}
//		else
//		{
//			UE_LOG(LogTemp, Error, TEXT("Parse Json Error! JsonString: %s"), *OriginJsonString);
//		}
//
//		int JsonEndSymbolIndex;
//		if (!JsonString.FindLastChar('}', JsonEndSymbolIndex))
//		{
//			UE_LOG(LogTemp, Error, TEXT("Parse Json Error! JsonString: %s"), *OriginJsonString);
//			break;
//		}
//		JsonString = JsonString.Mid(0, JsonEndSymbolIndex + 1);
//	}
//	return Result;
//}

//FMQMenuListResponse URabbitMqProtocol::ParseMQMenuListResponse(FString JsonString)
//{
//	FMQMenuListResponse Result;
//	Result.Init(JsonString);
//	return Result;
//}
//
//FMQComponentListResponse URabbitMqProtocol::ParseMQComponentListResponse(FString JsonString)
//{
//	FMQComponentListResponse Result;
//	Result.Init(JsonString);
//	return Result;
//}


