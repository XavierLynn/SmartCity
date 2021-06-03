// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Interfaces/IHttpRequest.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"

#include "DigitalFrameNetWorkBlueprintLibrary.generated.h"


DECLARE_DYNAMIC_DELEGATE_OneParam(FHttpResponseDelegate,FString,Result);


UENUM(BlueprintType)
enum EHttpVerb
{
	Get,
    Post,
};


UCLASS()
class PLATEMAP_API UDigitalFrameNetWorkBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category="Http")
	static void HttpRequest(const EHttpVerb HttpVerb, const FString& Url, const FString& PostData, FHttpResponseDelegate HttpResponseDelegate)
	{
		TMap<FString,FString> MapHeads;
		HttpRequestWithHeads(HttpVerb,Url,PostData,MapHeads,HttpResponseDelegate);
	}
	
	UFUNCTION(BlueprintCallable, Category="Http")
	static void HttpRequestWithHeads(const EHttpVerb HttpVerb, const FString& Url, const FString& PostData, const TMap<FString,FString>& MapHeads , FHttpResponseDelegate HttpResponseDelegate)
	{
		FHttpModule* HttpModule = &FHttpModule::Get();
#if ENGINE_MINOR_VERSION >= 26
		TSharedRef<IHttpRequest,ESPMode::ThreadSafe> HttpRequest = HttpModule->CreateRequest();
#else
		TSharedRef<IHttpRequest> HttpRequest = HttpModule->CreateRequest();
#endif

		UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EHttpVerb"), true);
		FString Verb(EnumPtr->GetNameByValue((int)HttpVerb).ToString());//转化相应枚举字符串
		
		HttpRequest->SetVerb(Verb);
		HttpRequest->SetURL(Url);
		
		for (auto& Elem :MapHeads)
			HttpRequest->SetHeader(Elem.Key, *Elem.Value);
	
		if (PostData.IsEmpty() == false)
			HttpRequest->SetContentAsString(PostData);

		HttpRequest->OnProcessRequestComplete().BindLambda([HttpResponseDelegate]( FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool Succeeded )
		{
			if (Succeeded && EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
				HttpResponseDelegate.ExecuteIfBound(HttpResponse->GetContentAsString() );
			
		});
		
		HttpRequest->ProcessRequest();
	}
};
