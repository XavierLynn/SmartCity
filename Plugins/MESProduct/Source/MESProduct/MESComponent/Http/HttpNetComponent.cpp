// Fill out your copyright notice in the Description page of Project Settings.


#include "HttpNetComponent.h"

// Sets default values for this component's properties
UHttpNetComponent::UHttpNetComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UHttpNetComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	HeartbeatResponseDelegate.BindUFunction(this, "OnHeartbeatResponse");
}

void UHttpNetComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction)
{
	if (IsPendingKillOrUnreachable() || !IsValidLowLevelFast())
	{
		return;
	}

	HeartbeatTimer += DeltaTime;
	if (HeartbeatTimer >= HeartbeatInterval)
	{
		HttpRequest(HeartbeatSubURL, "{}", true);
	}
}

void UHttpNetComponent::HttpRequestNew(FString Url, FHttpResponseDelegate Callback, TMap<FString, FString> AppendHead, FString HeadValue, FString Method, FString PostData)
{
	FHttpModule* HttpModule = &FHttpModule::Get();
#if ENGINE_MINOR_VERSION >= 26
	TSharedRef<IHttpRequest,ESPMode::ThreadSafe> HttpRequest = HttpModule->CreateRequest();
#else
	TSharedRef<IHttpRequest> HttpRequest = HttpModule->CreateRequest();
#endif

	HttpRequest->SetVerb(Method);

	HttpRequest->SetURL(Url);
	HttpRequest->SetHeader("Content-Type", HeadValue);

	for (auto& Elem :AppendHead)
	{
		HttpRequest->SetHeader(Elem.Key, *Elem.Value);
    }
	
	HttpRequest->SetHeader("Cookie", HttpCookie);
	if ( Method.Equals(TEXT("POST") ) )
		HttpRequest->SetContentAsString(PostData);

	
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UHttpNetComponent::HttpResultCPP);
	
	HttpRequest->ProcessRequest();
	OnHttpResponse = Callback;
}

void UHttpNetComponent::HttpCombineURL(FString MainURL, FString SubURL, FString & OutURL) const
{
	if (!MainURL.EndsWith("/"))
	{
		MainURL = MainURL.Append("/");
	}
	if (SubURL.StartsWith("/") && SubURL.Len() > 1)
	{
		SubURL = SubURL.Mid(1, SubURL.Len() - 1);
	}
	OutURL = HttpURL + SubURL;
}

void UHttpNetComponent::HttpResultCPP(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	if (!HttpRequest.IsValid() || !HttpResponse.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("failed to process http request"));
		//HttpResult(HttpResponse->GetURL(),"",false);
	}
	else if (bSucceeded && EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
	{
		const FString Result = HttpResponse->GetContentAsString();
		UE_LOG(LogTemp, Warning, TEXT("received http response: %s"), *Result);

		if (OnHttpResponse.IsBound())
		{
			OnHttpResponse.Execute(Result);
		}
		
		//HttpResult(HttpResponse->GetURL(),Result,true);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("received http response error: %d"), HttpResponse->GetResponseCode());
		//HttpResult(HttpResponse->GetURL(),"",false);
	}
}

void UHttpNetComponent::HttpRequestGet(FString Table, FString Limit)
{
	FHttpModule* HttpModule = &FHttpModule::Get();
#if ENGINE_MINOR_VERSION >= 26
    	TSharedRef<IHttpRequest,ESPMode::ThreadSafe> HttpRequest = HttpModule->CreateRequest();
#else
    	TSharedRef<IHttpRequest> HttpRequest = HttpModule->CreateRequest();
#endif
	HttpRequest->SetVerb("GET");

	FString TableStr = FGenericPlatformHttp::UrlEncode(Table);
	FString URL = HttpURL + "?table=" + TableStr + "&limit=" + Limit;
	HttpRequest->SetURL(URL);
	UE_LOG(LogTemp, Log, TEXT("get url: %s"), *URL);
	HttpRequest->SetHeader("Content-Type", "application/json");
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UHttpNetComponent::OnRequestComplete);
	HttpRequest->ProcessRequest();

	/*Request->SetHeader(TEXT("User-Agent"), TEXT("X-UnrealEngine-Agent"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json; charset=utf-8"));
	Request->SetHeader(TEXT("Accepts"), TEXT("application/json"));*/
}

void UHttpNetComponent::HttpRequestPost(FString Table, FString Limit)
{
	FHttpModule* HttpModule = &FHttpModule::Get();
#if ENGINE_MINOR_VERSION >= 26
    	TSharedRef<IHttpRequest,ESPMode::ThreadSafe> HttpRequest = HttpModule->CreateRequest();
#else
    	TSharedRef<IHttpRequest> HttpRequest = HttpModule->CreateRequest();
#endif
	HttpRequest->SetVerb("POST");
	HttpRequest->SetURL(HttpURL);
	HttpRequest->SetHeader("Content-Type", "application/json");

	FString ContentStr = "{\"table\":\"";
	ContentStr += Table;
	ContentStr += "\", \"limit\":\"";
	ContentStr += Limit;
	ContentStr += "\"}";
	HttpRequest->SetContentAsString(ContentStr);
	UE_LOG(LogTemp, Log, TEXT("post content: %s"), *ContentStr);

	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UHttpNetComponent::OnRequestComplete);
	HttpRequest->ProcessRequest();
}

void UHttpNetComponent::HttpRequest(FString SubURL, FString ContentString, bool IsHeartbeat/* = false*/)
{
	FHttpModule* HttpModule = &FHttpModule::Get();
#if ENGINE_MINOR_VERSION >= 26
    	TSharedRef<IHttpRequest,ESPMode::ThreadSafe> HttpRequest = HttpModule->CreateRequest();
#else
    	TSharedRef<IHttpRequest> HttpRequest = HttpModule->CreateRequest();
#endif
	HttpRequest->SetVerb("POST");
	FString URL;
	CombineURL(HttpURL, SubURL, URL);
	UE_LOG(LogTemp, Warning, TEXT("http url: %s"), *URL);
	UE_LOG(LogTemp, Warning, TEXT("ContentString: %s"), *ContentString);
	HttpRequest->SetURL(URL);
	HttpRequest->SetHeader("Content-Type", "application/json");
	HttpRequest->SetHeader("Cookie", HttpCookie);
	HttpRequest->SetContentAsString(ContentString);

	if (IsHeartbeat)
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UHttpNetComponent::OnHeartbeatRequestComplete);
	}
	else
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UHttpNetComponent::OnRequestComplete);
	}
	
	HttpRequest->ProcessRequest();
	HeartbeatTimer = 0.f;
}

void UHttpNetComponent::HttpRequestUpload(FString SubURL, FString Content, FString Boundary)
{
	FHttpModule* HttpModule = &FHttpModule::Get();
#if ENGINE_MINOR_VERSION >= 26
    	TSharedRef<IHttpRequest,ESPMode::ThreadSafe> HttpRequest = HttpModule->CreateRequest();
#else
    	TSharedRef<IHttpRequest> HttpRequest = HttpModule->CreateRequest();
#endif
	HttpRequest->SetVerb("POST");
	FString URL;
	CombineURL(HttpURL, SubURL, URL);
	UE_LOG(LogTemp, Warning, TEXT("http url: %s"), *URL);
	HttpRequest->SetURL(URL);
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("multipart/form-data; boundary =" + Boundary));
	HttpRequest->SetHeader("Cookie", HttpCookie);
	HttpRequest->SetContentAsString(Content);

	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UHttpNetComponent::OnRequestComplete);
	HttpRequest->ProcessRequest();

	HeartbeatTimer = 0.f;
}

FString UHttpNetComponent::MakeMD5Str(FString UserName, FString Password)
{
	FString MD5Str = Password + "{" + UserName + "}";
	FString MD5Result = FMD5::HashAnsiString(*MD5Str);
	return MD5Result;
}

void UHttpNetComponent::LoginRequest(FString SubURL, FHttpLoginRequest LoginRequest, FHttpResponseDelegate Callback)
{
	FString Credentials = MakeMD5Str(LoginRequest.UserName, LoginRequest.Password);
	OnHttpResponse = Callback;
	HttpRequest(SubURL, LoginRequest.ToString(Credentials));
}

void UHttpNetComponent::LogoutRequest(FString SubURL, FHttpLogoutRequest LogoutRequest, FHttpResponseDelegate Callback)
{
	OnHttpResponse = Callback;
	HttpRequest(SubURL, LogoutRequest.ToString());
}

void UHttpNetComponent::ProjectListRequest(FString SubURL, FHttpProjectListRequest ProjectListRequest, FHttpResponseDelegate Callback)
{
	OnHttpResponse = Callback;
	HttpRequest(SubURL, ProjectListRequest.ToString());
}

void UHttpNetComponent::CreateProjectRequest(FString SubURL, FHttpCreateProjectRequest CreateProjectRequest, FHttpResponseDelegate Callback)
{
	OnHttpResponse = Callback;
	CreateProjectRequest.password = MakeMD5Str(CreateProjectRequest.username, CreateProjectRequest.password);
	FString Boundary = GetFormBoundary();
	HttpRequestUpload(SubURL, CreateProjectRequest.ToString(Boundary), Boundary);
}

void UHttpNetComponent::UpdateProjectRequest(FString SubURL, FHttpUpdateProjectRequest UpdateProjectRequest, FHttpResponseDelegate Callback)
{
	OnHttpResponse = Callback;
	FString Boundary = GetFormBoundary();
	HttpRequestUpload(SubURL, UpdateProjectRequest.ToString(Boundary), Boundary);
}

void UHttpNetComponent::DeleteProjectRequest(FString SubURL, FHttpDeleteProjectRequest DeleteProjectRequest, FHttpResponseDelegate Callback)
{
	OnHttpResponse = Callback;
	HttpRequest(SubURL, DeleteProjectRequest.ToString());
}

void UHttpNetComponent::CopyProjectRequest(FString SubURL, FHttpCopyProjectRequest CopyProjectRequest, FHttpResponseDelegate Callback)
{
	OnHttpResponse = Callback;
	HttpRequest(SubURL, CopyProjectRequest.ToString());
}

void UHttpNetComponent::GetTicketIdRequest(FString SubURL, FHttpGetTicketIdRequest GetTicketIdRequest, FHttpResponseDelegate Callback)
{
	OnHttpResponse = Callback;
	HttpRequest(SubURL, GetTicketIdRequest.ToString());
}

void UHttpNetComponent::OnRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	if (!HttpRequest.IsValid() || !HttpResponse.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("failed to process http request"));
	}
	else if (bSucceeded && EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
	{
		//FString GetCookie = HttpRequest.Get()->GetHeader("Cookie");

		if (HttpCookie.IsEmpty())
		{
			FString GetCookie = HttpResponse.Get()->GetHeader("Set-Cookie");
			TArray<FString> CookieContents;
			GetCookie.ParseIntoArray(CookieContents, TEXT(";"));
			if (CookieContents.Num() > 0)
			{
				HttpCookie = CookieContents[0];
			}
			UE_LOG(LogTemp, Warning, TEXT("http cookie: %s"), *HttpCookie);
		}

		FString Result = HttpResponse->GetContentAsString();
		UE_LOG(LogTemp, Warning, TEXT("received http response: %s"), *Result);
		if (OnHttpResponse.IsBound())
		{
			OnHttpResponse.Execute(Result);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("received http response error: %d"), HttpResponse->GetResponseCode());
	}
}

FString UHttpNetComponent::GetFormBoundary()
{
	return "---------------------------" + FString::FromInt(FDateTime::Now().GetTicks());
}

void UHttpNetComponent::OnHeartbeatResponse(const FString & JsonString)
{
	UE_LOG(LogTemp, Warning, TEXT("http heartbeat response json string: %s"), *JsonString);
}

void UHttpNetComponent::CombineURL(FString MainURL, FString SubURL, FString & OutURL)
{
	if (!MainURL.EndsWith("/"))
	{
		MainURL = MainURL.Append("/");
	}
	if (SubURL.StartsWith("/") && SubURL.Len() > 1)
	{
		SubURL = SubURL.Mid(1, SubURL.Len() - 1);
	}
	OutURL = HttpURL + SubURL;
}

void UHttpNetComponent::OnHeartbeatRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	if (!HttpRequest.IsValid() || !HttpResponse.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("failed to process http heartbeat request"));
	}
	else if (bSucceeded && EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
	{
		//FString GetCookie = HttpRequest.Get()->GetHeader("Cookie");

		if (HttpCookie.IsEmpty())
		{
			FString GetCookie = HttpResponse.Get()->GetHeader("Set-Cookie");
			TArray<FString> CookieContents;
			GetCookie.ParseIntoArray(CookieContents, TEXT(";"));
			if (CookieContents.Num() > 0)
			{
				HttpCookie = CookieContents[0];
			}
			UE_LOG(LogTemp, Warning, TEXT("http heartbeat cookie: %s"), *HttpCookie);
		}

		FString Result = HttpResponse->GetContentAsString();
		UE_LOG(LogTemp, Warning, TEXT("received http heartbeat response: %s"), *Result);
		if (HeartbeatResponseDelegate.IsBound())
		{
			HeartbeatResponseDelegate.Execute(Result);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("received http heartbeat response error: %d"), HttpResponse->GetResponseCode());
		OnHeartBeatFailed.Broadcast();
	}
}

