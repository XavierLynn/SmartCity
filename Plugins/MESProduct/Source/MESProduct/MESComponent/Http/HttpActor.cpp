// Fill out your copyright notice in the Description page of Project Settings.


#include "HttpActor.h"
#include "Engine/World.h"


AHttpActor* AHttpActor::Instance = nullptr;

// Sets default values
AHttpActor::AHttpActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AHttpActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AHttpActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Instance = nullptr;
	if (OnHttpResponse.IsBound())
	{
		OnHttpResponse.Unbind();
	}
	Super::EndPlay(EndPlayReason);
}

// Called every frame
void AHttpActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHttpActor::HttpRequestGet(FString Url, FHttpResponseDelegate Callback)
{
	if (!GWorld->GetWorld())
	{
		return;
	}

	if (Instance == nullptr)
	{
		Instance = GWorld->GetWorld()->SpawnActor<AHttpActor>();
	}

	FHttpModule* HttpModule = &FHttpModule::Get();
#if ENGINE_MINOR_VERSION >= 26
    	TSharedRef<IHttpRequest,ESPMode::ThreadSafe> HttpRequest = HttpModule->CreateRequest();
#else
    	TSharedRef<IHttpRequest> HttpRequest = HttpModule->CreateRequest();
#endif
	HttpRequest->SetVerb("GET");
	HttpRequest->SetURL(Url);
	UE_LOG(LogTemp, Warning, TEXT("http get url: %s"), *Url);
	HttpRequest->SetHeader("Content-Type", "application/json");
	HttpRequest->OnProcessRequestComplete().BindUObject(Instance, &AHttpActor::OnRequestComplete);
	HttpRequest->ProcessRequest();
	Instance->OnHttpResponse = Callback;
}

void AHttpActor::HttpRequestPost(FString Url, FString ContentString, FHttpResponseDelegate Callback)
{
	if (!GWorld->GetWorld())
	{
		return;
	}

	if (Instance == nullptr)
	{
		Instance = GWorld->GetWorld()->SpawnActor<AHttpActor>();
	}

	FHttpModule* HttpModule = &FHttpModule::Get();
#if ENGINE_MINOR_VERSION >= 26
    	TSharedRef<IHttpRequest,ESPMode::ThreadSafe> HttpRequest = HttpModule->CreateRequest();
#else
    	TSharedRef<IHttpRequest> HttpRequest = HttpModule->CreateRequest();
#endif
	HttpRequest->SetVerb("POST");
	HttpRequest->SetURL(Url);
	HttpRequest->SetHeader("Content-Type", "application/json");
	HttpRequest->SetContentAsString(ContentString);
	UE_LOG(LogTemp, Warning, TEXT("http post content: %s"), *ContentString);

	HttpRequest->OnProcessRequestComplete().BindUObject(Instance, &AHttpActor::OnRequestComplete);
	HttpRequest->ProcessRequest();
	Instance->OnHttpResponse = Callback;
}

void AHttpActor::OnRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	if (!HttpRequest.IsValid() || !HttpResponse.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("failed to process http request"));
	}
	else if (bSucceeded && EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
	{
		FString Result = HttpResponse->GetContentAsString();
		UE_LOG(LogTemp, Warning, TEXT("received http response: %s"), *Result);
		if (OnHttpResponse.IsBound())
		{
			TSharedPtr<FJsonObject> TempJsonObject;
			TSharedRef< TJsonReader<TCHAR> > JsonReader = TJsonReaderFactory<TCHAR>::Create(Result);
			if (FJsonSerializer::Deserialize(JsonReader, TempJsonObject))
			{
				FString TargetPrettyJson;
				TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> JsonWriter = TJsonWriterFactory<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>::Create(&TargetPrettyJson);

				if (TempJsonObject.IsValid() && FJsonSerializer::Serialize(TempJsonObject.ToSharedRef(), JsonWriter))
				{
					OnHttpResponse.Execute(TargetPrettyJson);
					OnHttpResponse.Unbind();
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("received http response error: %d"), HttpResponse->GetResponseCode());
	}
}

