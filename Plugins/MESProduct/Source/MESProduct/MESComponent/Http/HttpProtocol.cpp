
#include "HttpProtocol.h"

/*
namespace HttpProtocolTool
{
	void AppendJsonStr(FString& Source, FString Key, FString Value)
	{
		Source.Append("\",\"");
		Source.Append(Key);
		Source.Append("\":\"");
		Source.Append(Value);
	}
	void AppendJsonInt(FString& Source, FString Key, int Value)
	{
		Source.Append("\",\"");
		Source.Append(Key);
		Source.Append("\":\"");
		Source.Append(FString::FromInt(Value));
	}
	void AppendJsonfloat(FString& Source, FString Key, float Value)
	{
		Source.Append("\",\"");
		Source.Append(Key);
		Source.Append("\":\"");
		Source.Append(FString::SanitizeFloat(Value));
	}
	void AppendJsonBool(FString& Source, FString Key, bool Value)
	{
		Source.Append("\",\"");
		Source.Append(Key);
		Source.Append("\":\"");
		Source.Append(Value ? "1" : "0");
	}
}
*/

#define MES_HTTP_RESPONSE_DEFINE(PROTOCOL_NAME) \
FHttp##PROTOCOL_NAME##Response UHttpProtocol::Parse##PROTOCOL_NAME##Response(FString JsonString) \
{ \
	FHttp##PROTOCOL_NAME##Response Result; \
	if (!FJsonObjectConverter::JsonObjectStringToUStruct<FHttp##PROTOCOL_NAME##Response>(JsonString, &Result, 0, 0)) \
	{ \
		int JsonEndSymbolIndex; \
		JsonString.FindLastChar('}', JsonEndSymbolIndex); \
		JsonString = JsonString.Mid(0, JsonEndSymbolIndex + 1); \
		FJsonObjectConverter::JsonObjectStringToUStruct<FHttp##PROTOCOL_NAME##Response>(JsonString, &Result, 0, 0); \
	} \
	return Result; \
}

MES_HTTP_RESPONSE_DEFINE(Login)
MES_HTTP_RESPONSE_DEFINE(Logout)
MES_HTTP_RESPONSE_DEFINE(ProjectList)
MES_HTTP_RESPONSE_DEFINE(CreateProject)
MES_HTTP_RESPONSE_DEFINE(UpdateProject)
MES_HTTP_RESPONSE_DEFINE(DeleteProject)
MES_HTTP_RESPONSE_DEFINE(CopyProject)
MES_HTTP_RESPONSE_DEFINE(GetTicketId)

//FHttpLoginResponse UHttpProtocol::ParseLoginResponse(FString JsonString)
//{
//	FHttpLoginResponse Result;
//	FJsonObjectConverter::JsonObjectStringToUStruct<FHttpLoginResponse>(JsonString, &Result, 0, 0);
//	return Result;
//}
//
//FHttpLogoutResponse UHttpProtocol::ParseLogoutResponse(FString JsonString)
//{
//	FHttpLogoutResponse Result;
//	FJsonObjectConverter::JsonObjectStringToUStruct<FHttpLogoutResponse>(JsonString, &Result, 0, 0);
//	return Result;
//}
//
//FHttpProjectListResponse UHttpProtocol::ParseProjectListResponse(FString JsonString)
//{
//	FHttpProjectListResponse Result;
//	FJsonObjectConverter::JsonObjectStringToUStruct<FHttpProjectListResponse>(JsonString, &Result, 0, 0);
//	return Result;
//}
