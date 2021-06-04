// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "JsonUtilities/Public/JsonUtilities.h"
#include "HttpProtocol.generated.h"


/**
 * 
 */

/*
namespace HttpProtocolTool
{
	void AppendJsonStr(FString& Source, FString Key, FString Value);
	void AppendJsonInt(FString& Source, FString Key, int Value);
	void AppendJsonfloat(FString& Source, FString Key, float Value);
	void AppendJsonBool(FString& Source, FString Key, bool Value);
}
*/

/********** Http Login Start  *********/
USTRUCT(BlueprintType)
struct FHttpLoginRequest
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		FString UserName;
	UPROPERTY(BlueprintReadWrite)
		FString Password;
	UPROPERTY(BlueprintReadWrite)
		int ClientType;
	UPROPERTY(BlueprintReadWrite)
		bool AutoLogin;

	FString ToString(FString Credentials)
	{
		FString Result("{");
		/*HttpProtocolTool::AppendJsonStr(Result, "consumer", UserName);
		HttpProtocolTool::AppendJsonStr(Result, "credentials", Credentials);
		HttpProtocolTool::AppendJsonInt(Result, "clientType", ClientType);
		HttpProtocolTool::AppendJsonBool(Result, "auto", AutoLogin);*/
		Result.Append("\"consumer\":\"");
		Result.Append(UserName);
		Result.Append("\",\"credentials\":\"");
		Result.Append(Credentials);
		Result.Append("\",\"clientType\":");
		Result.Append(FString::FromInt(ClientType));
		Result.Append(",\"auto\":");
		Result.Append(AutoLogin ? "1" : "0");
		Result.Append("}");
		return Result;
	}
};
USTRUCT(BlueprintType)
struct FHttpLoginData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		FString token;
};
USTRUCT(BlueprintType)
struct FHttpLoginResponse
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		int status;
	UPROPERTY(BlueprintReadWrite)
		FString message;
	UPROPERTY(BlueprintReadWrite)
		FHttpLoginData data;
};
/********** Http Login End  *********/


/********** Http Logout Start  *********/
USTRUCT(BlueprintType)
struct FHttpLogoutRequest
{
	GENERATED_BODY()

	// has no body

	FString ToString()
	{
		FString Result("{}");
		return Result;
	}
};
USTRUCT(BlueprintType)
struct FHttpLogoutResponse
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		int status;
	UPROPERTY(BlueprintReadWrite)
		FString message;
};
/********** Http Logout End  *********/



/********** Http ProjectList Start  *********/
USTRUCT(BlueprintType)
struct FHttpProjectListRequest
{
	GENERATED_BODY()

	// has no body

	FString ToString()
	{
		FString Result("{}");
		return Result;
	}
};
USTRUCT(BlueprintType)
struct FHttpProjectData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		int projectId;
	UPROPERTY(BlueprintReadWrite)
		int projectCode;
	UPROPERTY(BlueprintReadWrite)
		FString projectName;
	UPROPERTY(BlueprintReadWrite)
		int projectVersion;
	UPROPERTY(BlueprintReadWrite)
		FString projectVersionName;
	UPROPERTY(BlueprintReadWrite)
		FString host;
	UPROPERTY(BlueprintReadWrite)
		FString port;
	UPROPERTY(BlueprintReadWrite)
		FString exchange;
	UPROPERTY(BlueprintReadWrite)
		FString username;
	UPROPERTY(BlueprintReadWrite)
		int onlineStatus;
	UPROPERTY(BlueprintReadWrite)
		FString password;
	UPROPERTY(BlueprintReadWrite)
		int width;
	UPROPERTY(BlueprintReadWrite)
		int height;
	UPROPERTY(BlueprintReadWrite)
		FString thumbnailPathe;
	UPROPERTY(BlueprintReadWrite)
		FString createUserId;
	UPROPERTY(BlueprintReadWrite)
		FString createTime;
	
};
USTRUCT(BlueprintType)
struct FHttpProjectListResponse
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		int status;
	UPROPERTY(BlueprintReadWrite)
		FString message;
	UPROPERTY(BlueprintReadWrite)
		TArray<FHttpProjectData> data;
};
/********** Http ProjectList End  *********/




/********** Http CreateProject Start  *********/
USTRUCT(BlueprintType)
struct FHttpCreateProjectRequest
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		int projectId = -1;
	UPROPERTY(BlueprintReadWrite)
		FString projectCode;
	UPROPERTY(BlueprintReadWrite)
		FString projectName;
	UPROPERTY(BlueprintReadWrite)
		FString host;
	UPROPERTY(BlueprintReadWrite)
		FString port;
	UPROPERTY(BlueprintReadWrite)
		FString exchange;
	UPROPERTY(BlueprintReadWrite)
	FString username;
	UPROPERTY(BlueprintReadWrite)
		FString password;
	UPROPERTY(BlueprintReadWrite)
		int width;
	UPROPERTY(BlueprintReadWrite)
		int height;
	UPROPERTY(BlueprintReadWrite)
		FString thumbnailFile;

	FString ToString(const FString& Boundary)
	{
		FString Content;
		
		//FJsonObjectConverter::UStructToJsonObjectString<FHttpCreateProjectRequest>(*this, Content);

		/*Content.Append("\r\nContent-Disposition: form-data; name=\"projectId\"\r\n");
		Content.Append(FString::FromInt(projectId));
		Content.Append("\r\nContent-Disposition: form-data; name=\"projectCode\"\r\n");
		Content.Append(projectCode);
		Content.Append("\r\nContent-Disposition: form-data; name=\"projectName\"\r\n");
		Content.Append(projectName);
		Content.Append("\r\nContent-Disposition: form-data; name=\"host\"\r\n");
		Content.Append(host);
		Content.Append("\r\nContent-Disposition: form-data; name=\"port\"\r\n");
		Content.Append(port);
		Content.Append("\r\nContent-Disposition: form-data; name=\"exchange\"\r\n");
		Content.Append(exchange);
		Content.Append("\r\nContent-Disposition: form-data; name=\"username\"\r\n");
		Content.Append(username);
		Content.Append("\r\nContent-Disposition: form-data; name=\"password\"\r\n");
		Content.Append(password);
		Content.Append("\r\nContent-Disposition: form-data; name=\"width\"\r\n");
		Content.Append(FString::FromInt(width));
		Content.Append("\r\nContent-Disposition: form-data; name=\"height\"\r\n");
		Content.Append(FString::FromInt(height));
		Content.Append("\r\nContent-Disposition: form-data; name=\"thumbnailFile\"\r\n");
		Content.Append(thumbnailFile);*/

		Append(Content, "projectId", FString::FromInt(projectId), Boundary);
		Append(Content, "projectCode", projectCode, Boundary);
		Append(Content, "projectName", projectName, Boundary);
		Append(Content, "host", host, Boundary);
		Append(Content, "port", port, Boundary);
		Append(Content, "exchange", exchange, Boundary);
		Append(Content, "username", username, Boundary);
		Append(Content, "password", password, Boundary);
		Append(Content, "width", FString::FromInt(width), Boundary);
		Append(Content, "height", FString::FromInt(height), Boundary);

		/*TArray<uint8> FileContent;
		if (FFileHelper::LoadFileToArray(FileContent, *thumbnailFile))
		{
			Append(Content, "thumbnailFile", thumbnailFile, Boundary);
		}*/

		FString EndBoundary = "\r\n--" + Boundary + "--\r\n";
		Content.Append(EndBoundary);
		UE_LOG(LogTemp, Warning, TEXT("Content: %s"), *Content);

		return Content;
	}

	void Append(FString& Content, FString Key, FString Value, const FString& Boundary)
	{
		Content.Append("\r\n--" + Boundary + "\r\n");
		Content.Append("Content-Disposition: form-data; name=\"" + Key + "\"");
		Content.Append("\r\n\r\n");
		Content.Append(Value);
	}
};
USTRUCT(BlueprintType)
struct FHttpCreateProjectData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		int projectId;
	UPROPERTY(BlueprintReadWrite)
		int projectCode;
	UPROPERTY(BlueprintReadWrite)
		FString projectName;
	UPROPERTY(BlueprintReadWrite)
		int projectVersion;
	UPROPERTY(BlueprintReadWrite)
		FString projectVersionName;
	UPROPERTY(BlueprintReadWrite)
		FString host;
	UPROPERTY(BlueprintReadWrite)
		FString port;
	UPROPERTY(BlueprintReadWrite)
		FString exchange;
	UPROPERTY(BlueprintReadWrite)
		FString username;
	UPROPERTY(BlueprintReadWrite)
		int onlineStatus;
	UPROPERTY(BlueprintReadWrite)
		FString password;
	UPROPERTY(BlueprintReadWrite)
		int width;
	UPROPERTY(BlueprintReadWrite)
		int height;
	UPROPERTY(BlueprintReadWrite)
		FString thumbnailPathe;
	UPROPERTY(BlueprintReadWrite)
		FString createUserName;
	UPROPERTY(BlueprintReadWrite)
		FString createTimeStr;
	UPROPERTY(BlueprintReadWrite)
		FString createTime;
};
USTRUCT(BlueprintType)
struct FHttpCreateProjectResponse
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		int status;
	UPROPERTY(BlueprintReadWrite)
		FString message;
	UPROPERTY(BlueprintReadWrite)
		FHttpCreateProjectData data;
};
/********** Http CreateProject End  *********/




/********** Http UpdateProject Start  *********/
USTRUCT(BlueprintType)
struct FHttpUpdateProjectRequest
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		int projectId;
	UPROPERTY(BlueprintReadWrite)
		FString projectCode;
	UPROPERTY(BlueprintReadWrite)
		FString projectName;
	UPROPERTY(BlueprintReadWrite)
		FString host;
	UPROPERTY(BlueprintReadWrite)
		FString port;
	UPROPERTY(BlueprintReadWrite)
		FString exchange;
	UPROPERTY(BlueprintReadWrite)
		FString username;
	UPROPERTY(BlueprintReadWrite)
		FString password;
	UPROPERTY(BlueprintReadWrite)
		int width;
	UPROPERTY(BlueprintReadWrite)
		int height;
	UPROPERTY(BlueprintReadWrite)
		FString thumbnailFile;

	FString ToString(const FString& Boundary)
	{
		FString Content;

		Append(Content, "projectId", FString::FromInt(projectId), Boundary);
		Append(Content, "projectCode", projectCode, Boundary);
		Append(Content, "projectName", projectName, Boundary);
		Append(Content, "host", host, Boundary);
		Append(Content, "port", port, Boundary);
		Append(Content, "exchange", exchange, Boundary);
		Append(Content, "username", username, Boundary);
		Append(Content, "password", password, Boundary);
		Append(Content, "width", FString::FromInt(width), Boundary);
		Append(Content, "height", FString::FromInt(height), Boundary);

		/*TArray<uint8> FileContent;
		if (FFileHelper::LoadFileToArray(FileContent, *thumbnailFile))
		{
			Append(Content, "thumbnailFile", thumbnailFile, Boundary);
		}*/

		FString EndBoundary = "\r\n--" + Boundary + "--\r\n";
		Content.Append(EndBoundary);
		UE_LOG(LogTemp, Warning, TEXT("Content: %s"), *Content);

		return Content;
	}

	void Append(FString& Content, FString Key, FString Value, const FString& Boundary)
	{
		Content.Append("\r\n--" + Boundary + "\r\n");
		Content.Append("Content-Disposition: form-data; name=\"" + Key + "\"");
		Content.Append("\r\n\r\n");
		Content.Append(Value);
	}
};
USTRUCT(BlueprintType)
struct FHttpUpdateProjectData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		int projectId;
	UPROPERTY(BlueprintReadWrite)
		int projectCode;
	UPROPERTY(BlueprintReadWrite)
		FString projectName;
	UPROPERTY(BlueprintReadWrite)
		int projectVersion;
	UPROPERTY(BlueprintReadWrite)
		FString projectVersionName;
	UPROPERTY(BlueprintReadWrite)
		FString host;
	UPROPERTY(BlueprintReadWrite)
		FString port;
	UPROPERTY(BlueprintReadWrite)
		FString exchange;
	UPROPERTY(BlueprintReadWrite)
		FString username;
	UPROPERTY(BlueprintReadWrite)
		int onlineStatus;
	UPROPERTY(BlueprintReadWrite)
		FString password;
	UPROPERTY(BlueprintReadWrite)
		int width;
	UPROPERTY(BlueprintReadWrite)
		int height;
	UPROPERTY(BlueprintReadWrite)
		FString thumbnailPathe;
	UPROPERTY(BlueprintReadWrite)
		FString createUserId;
	UPROPERTY(BlueprintReadWrite)
		FString createTimeStr;
	UPROPERTY(BlueprintReadWrite)
		FString createTime;
};
USTRUCT(BlueprintType)
struct FHttpUpdateProjectResponse
{
	GENERATED_BODY()

		UPROPERTY(BlueprintReadWrite)
		int status;
	UPROPERTY(BlueprintReadWrite)
		FString message;
	UPROPERTY(BlueprintReadWrite)
		FHttpUpdateProjectData data;
};
/********** Http UpdateProject End  *********/





/********** Http DeleteProject Start  *********/
USTRUCT(BlueprintType)
struct FHttpDeleteProjectRequest
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		int projectId;

	FString ToString()
	{
		FString Result;
		FJsonObjectConverter::UStructToJsonObjectString<FHttpDeleteProjectRequest>(*this, Result);
		return Result;
	}
};
USTRUCT(BlueprintType)
struct FHttpDeleteProjectResponse
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		int status;
	UPROPERTY(BlueprintReadWrite)
		FString message;
	UPROPERTY(BlueprintReadWrite)
		FString data;
};
/********** Http DeleteProject End  *********/





/********** Http CopyProject Start  *********/
USTRUCT(BlueprintType)
struct FHttpCopyProjectRequest
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		int projectId;

	FString ToString()
	{
		FString Result;
		FJsonObjectConverter::UStructToJsonObjectString<FHttpCopyProjectRequest>(*this, Result);
		return Result;
	}
};
USTRUCT(BlueprintType)
struct FHttpCopyProjectData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		int projectId;
	UPROPERTY(BlueprintReadWrite)
		int projectCode;
	UPROPERTY(BlueprintReadWrite)
		FString projectName;
	UPROPERTY(BlueprintReadWrite)
		int projectVersion;
	UPROPERTY(BlueprintReadWrite)
		FString projectVersionName;
	UPROPERTY(BlueprintReadWrite)
		FString host;
	UPROPERTY(BlueprintReadWrite)
		FString port;
	UPROPERTY(BlueprintReadWrite)
		FString exchange;
	UPROPERTY(BlueprintReadWrite)
		FString username;
	UPROPERTY(BlueprintReadWrite)
		int onlineStatus;
	UPROPERTY(BlueprintReadWrite)
		FString password;
	UPROPERTY(BlueprintReadWrite)
		int width;
	UPROPERTY(BlueprintReadWrite)
		int height;
	UPROPERTY(BlueprintReadWrite)
		FString thumbnailPathe;
	UPROPERTY(BlueprintReadWrite)
		FString createUserName;
	UPROPERTY(BlueprintReadWrite)
		FString createTimeStr;
	UPROPERTY(BlueprintReadWrite)
		FString createTime;
};
USTRUCT(BlueprintType)
struct FHttpCopyProjectResponse
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		int status;
	UPROPERTY(BlueprintReadWrite)
		FString message;
	UPROPERTY(BlueprintReadWrite)
		FHttpCopyProjectData data;
};
/********** Http CopyProject End  *********/




/********** Http GetTicketId Start  *********/
USTRUCT(BlueprintType)
struct FHttpGetTicketIdRequest
{
	GENERATED_BODY()

		UPROPERTY(BlueprintReadWrite)
		FString projectId;

	FString ToString()
	{
		FString Result;
		FJsonObjectConverter::UStructToJsonObjectString<FHttpGetTicketIdRequest>(*this, Result);
		return Result;
	}
};
USTRUCT(BlueprintType)
struct FHttpGetTicketIdData
{
	GENERATED_BODY()

		UPROPERTY(BlueprintReadWrite)
		FString ticketId;
};
USTRUCT(BlueprintType)
struct FHttpGetTicketIdResponse
{
	GENERATED_BODY()

		UPROPERTY(BlueprintReadWrite)
		int status;
	UPROPERTY(BlueprintReadWrite)
		FString message;
	UPROPERTY(BlueprintReadWrite)
		FHttpGetTicketIdData data;
};
/********** Http GetTicketId End  *********/




UCLASS()
class MESPRODUCT_API UHttpProtocol : public UObject
{
	GENERATED_BODY()


public:
	UFUNCTION(BlueprintCallable)
		static FHttpLoginResponse ParseLoginResponse(FString JsonString);
	UFUNCTION(BlueprintCallable)
		static FHttpLogoutResponse ParseLogoutResponse(FString JsonString);
	UFUNCTION(BlueprintCallable)
		static FHttpProjectListResponse ParseProjectListResponse(FString JsonString);
	UFUNCTION(BlueprintCallable)
		static FHttpCreateProjectResponse ParseCreateProjectResponse(FString JsonString);
	UFUNCTION(BlueprintCallable)
		static FHttpUpdateProjectResponse ParseUpdateProjectResponse(FString JsonString);
	UFUNCTION(BlueprintCallable)
		static FHttpDeleteProjectResponse ParseDeleteProjectResponse(FString JsonString);
	UFUNCTION(BlueprintCallable)
		static FHttpCopyProjectResponse ParseCopyProjectResponse(FString JsonString);
	UFUNCTION(BlueprintCallable)
		static FHttpGetTicketIdResponse ParseGetTicketIdResponse(FString JsonString);
};
