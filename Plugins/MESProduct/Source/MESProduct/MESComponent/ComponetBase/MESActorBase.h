// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MESProduct\MESComponent\ComponetBase\MESComponetBase.h"
#include "MESActorBase.generated.h"

USTRUCT(BlueprintType)
struct FActorBaseData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (DisplayName = "UUID"))
	int64 UUID;//需要通过Sqlite得到唯一ID

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "组件名字"))
		FString ComponetName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "显示"))
		bool Visible = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "颜色Index"))
		int ColorIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "样式ID"))
		int StyleID;//通过StyleID 找到 组件的路径。然后根据路径在进行加载位置

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "透明度"))
		float BgAlpha = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "位置"))
		FVector Pos;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "旋转"))
		FQuat Rotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "大小"))
		FVector Size = FVector(1.f,1.f,1.f);
};

UCLASS()
class MESPRODUCT_API AMESActorBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMESActorBase();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MES组件属性", Meta = (DisplayName = "基础属性"))
		FActorBaseData CommonData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MES组件属性", Meta = (DisplayName = "数据源类型"))
		EDataSourceType DataSourceType = EDataSourceType::NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MES组件属性", Meta = (DisplayName = "数据分组"))
		FString DataGroup;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MES组件属性", Meta = (DisplayName = "表名"))
		FString TableName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MES组件属性", Meta = (DisplayName = "组件配置"))
		TMap<FString, FMESCustomConfig> CustomConfig;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MES组件属性", Meta = (DisplayName = "事件列表"))
		TArray<FMESEvents> Events;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "MES组件属性", Meta = (DisplayName = "字段绑定"))
		TMap<FString, FDataBind> DataBinds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "MES组件属性", Meta = (DisplayName = "筛选规则"))
		TArray<FFilterRule> FiterRules;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "MES组件属性", Meta = (DisplayName = "排序字段"))
		TArray<FDataSort> Sorts;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "MES组件属性", Meta = (DisplayName = "数据最大条数"))
		int MaxNum = 5000;

	//Not Save Data
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "MES本地属性", Meta = (DisplayName = "表格数据"))
		FTableData TableData;

	// make sure param of target funtion or event same like this:
	// param1:UObject* Sender
	// param2:FString Param
	UFUNCTION(BlueprintCallable, Category = "MESFunc")
		void ParseEvent(UObject* InnerObject, FString FunctionName, FString Param);

	UFUNCTION(BlueprintCallable, Category = "MESActorBase")
		void SetFinishForModel();

	UFUNCTION(BlueprintCallable, Category = "MESActorBase")
		void GenUUID();
	
	UFUNCTION(BlueprintCallable, Category = "MESActorBase")
		void SetTransformInfo();

	UFUNCTION(BlueprintCallable, Category = "MESFunc")
		void BindGetComponetData(FUpdateDataSingleDelegate CallBack);

	UFUNCTION(BlueprintCallable, Category = "MESFunc")
		void UnBindGetComponetData(FUpdateDataSingleDelegate CallBack);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MESFunc")
		int64 GetFilterRuleId();

	UFUNCTION(BlueprintCallable, Category = "MESFunc")
		TArray<FString>	GetDataColoumn(FString FieldName);

	UFUNCTION(BlueprintCallable, Category = "MESFunc")
		TArray<FTableRowData> GetDataColoumns(FString FieldName);

	UFUNCTION(BlueprintCallable, Category = "MESFunc")
		UMESComponetData* ConvertMESComponetData();

	UFUNCTION(BlueprintCallable, Category = "MESFunc")
		void SetMESComponetData(UMESComponetData* data);

	void GetComponetData();

	void HttpRequest(FString url, FString Methond);//当前仅仅支持 GET方式 后期会考虑 POST方式
	void OnRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	void HandleJson(FString Json);
	TArray<int> GetDataColoumnIndex(FString FieldName);

	FString GetFormBoundary();
	FString HttpCookie;

	FUpdateDataDelegate UpdateDataCallBack;
private:
	UFUNCTION()
		void OnDACColumnInfoResponse(FString JsonString);
	UFUNCTION()
		void OnDACTableDataResponse(FString JsonString);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnReceiveData();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
