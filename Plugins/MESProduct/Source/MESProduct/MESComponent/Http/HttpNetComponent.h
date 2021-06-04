// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Http.h"
#include "HttpProtocol.h"
#include "HttpNetComponent.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FHttpResponseDelegate, FString, JsonString);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHeartBeatFailed);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MESPRODUCT_API UHttpNetComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHttpNetComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

public:
	UFUNCTION(BlueprintCallable)
        void HttpRequestNew(FString SubURL, FHttpResponseDelegate Callback,TMap<FString,FString> AppendHead ,FString HeadValue="application/json",FString Method = "GET",FString PostData="" );

	void	HttpResultCPP(FHttpRequestPtr RequestPtr, FHttpResponsePtr ResponsePtr, bool bSuccess);

	void	HttpCombineURL(FString MainURL, FString SubURL, FString& OutURL) const;
	
	UFUNCTION(BlueprintCallable)
		void HttpRequestGet(FString Table, FString Limit);
	UFUNCTION(BlueprintCallable)
		void HttpRequestPost(FString Table, FString Limit);

	UFUNCTION(BlueprintCallable)
		void HttpRequest(FString SubURL, FString ContentString, bool IsHeartbeat = false);
	UFUNCTION(BlueprintCallable)
		void HttpRequestUpload(FString SubURL, FString Content, FString Boundary);
	UFUNCTION(BlueprintCallable)
		FString MakeMD5Str(FString UserName, FString Password);

	UFUNCTION(BlueprintCallable)
		void LoginRequest(FString SubURL, FHttpLoginRequest LoginRequest, FHttpResponseDelegate Callback);
	UFUNCTION(BlueprintCallable)
		void LogoutRequest(FString SubURL, FHttpLogoutRequest LogoutRequest, FHttpResponseDelegate Callback);
	UFUNCTION(BlueprintCallable)
		void ProjectListRequest(FString SubURL, FHttpProjectListRequest ProjectListRequest, FHttpResponseDelegate Callback);
	UFUNCTION(BlueprintCallable)
		void CreateProjectRequest(FString SubURL, FHttpCreateProjectRequest CreateProjectRequest, FHttpResponseDelegate Callback);
	UFUNCTION(BlueprintCallable)
		void UpdateProjectRequest(FString SubURL, FHttpUpdateProjectRequest UpdateProjectRequest, FHttpResponseDelegate Callback);
	UFUNCTION(BlueprintCallable)
		void DeleteProjectRequest(FString SubURL, FHttpDeleteProjectRequest DeleteProjectRequest, FHttpResponseDelegate Callback);
	UFUNCTION(BlueprintCallable)
		void CopyProjectRequest(FString SubURL, FHttpCopyProjectRequest CopyProjectRequest, FHttpResponseDelegate Callback);
	UFUNCTION(BlueprintCallable)
		void GetTicketIdRequest(FString SubURL, FHttpGetTicketIdRequest GetTicketIdRequest, FHttpResponseDelegate Callback);

	UPROPERTY(BlueprintAssignable, Category = "Components|HeartBeat")
		FOnHeartBeatFailed OnHeartBeatFailed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString HttpURL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString HeartbeatSubURL;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float HeartbeatInterval = 60.f;

private:

	void OnRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	FString GetFormBoundary();
	FString HttpCookie;
	FHttpResponseDelegate OnHttpResponse;

	float HeartbeatTimer = 0.f;
	UFUNCTION()
		void OnHeartbeatResponse(const FString& JsonString);
	FHttpResponseDelegate HeartbeatResponseDelegate;

	void CombineURL(FString MainURL, FString SubURL, FString& OutURL);
	void OnHeartbeatRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
};
