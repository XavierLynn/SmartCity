// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MESProduct\MESComponent\ComponetBase\MESComponetData.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Http.h"
#include "UMG\Public\Components\Image.h"
#include "MESComponetBase.generated.h"

UENUM(BlueprintType)
enum class EMESComponetEventType : uint8
{
	Componet = 0 UMETA(DisplayName = "组件"),
	Global = 1 UMETA(DisplayName = "全局"),
	Camera = 2 UMETA(DisplayName = "相机"),
	Page = 3 UMETA(DisplayName = "页面")
};

USTRUCT(BlueprintType)
struct FComponetEvent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "事件类型"))
		EMESComponetEventType EventType;//相机，页面，组件，全局

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Meta = (DisplayName = "事件名"))
		FString EventName;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Meta = (DisplayName = "参数"))
		FString EventParam;
};

USTRUCT(BlueprintType)
struct FMESEvents
{
	GENERATED_BODY()


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "触发方法"))
	FString TriggerMethod;//这个是由组件开发者自己定义的。

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "事件列表"))
	TArray<FComponetEvent> Events;
};

USTRUCT(BlueprintType)
struct FMESCustomConfig
{
	GENERATED_BODY()
	//字段名是Key，字段类型，字段值，分组   根据字段类型来进行动态反序列化展现方式
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "类别"))
	FString Category;//这个是由组件开发者。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "字段类型"))
	FString FiledType;//这个决定了组件属性的表现样式
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "字段值"))
	FString FiledValue;//值
};

UENUM(BlueprintType)
enum class EMESComponetType : uint8
{
	UIComponet = 0,
	Model3D = 1,
	PointComponet = 2,
};

USTRUCT(BlueprintType)
struct FComponetBaseData
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "Depth"))
		int Depth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "ParentUUID"))
		int64 ParentUUID = 0;//这个保存了父节点的依赖关系

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "位置"))
		FVector2D Pos;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "尺寸"))
		FVector2D Size = FVector2D(400,200);
};

USTRUCT(BlueprintType)
struct FColorArray
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (DisplayName = "颜色列"))
		TArray<FColor> ColorList;
};

class UMESComponetBase;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSelectComponetDelegate, UMESComponetBase*, componet);
DECLARE_DYNAMIC_DELEGATE_OneParam(FSelectSingleDelegate, UMESComponetBase*, componet);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FComponetMoveDelegae, FVector2D, pos);
DECLARE_DYNAMIC_DELEGATE_OneParam(FComonetMoveSingleDelegate, FVector2D, pos);

/**
 * 
 */
UCLASS()
class MESPRODUCT_API UMESComponetBase : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MES组件属性", Meta = (DisplayName = "基础属性"))
		FComponetBaseData CommonData;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite,AdvancedDisplay, Category = "MES组件属性", Meta = (DisplayName = "字段绑定"))
		TMap<FString,FDataBind> DataBinds;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,AdvancedDisplay, Category = "MES组件属性", Meta = (DisplayName = "所有筛选规则"))
		TArray<FFilterRule> FiterRules;
	
	//按且筛选还是按或筛选
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "MES组件属性")
		bool IsAND=true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "MES组件属性", Meta = (DisplayName = "排序字段"))
		TArray<FDataSort> Sorts;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,AdvancedDisplay, Category = "MES组件属性", Meta = (DisplayName = "数据最大条数"))
		int MaxNum = 5000;

	//========================================================================================================================
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MES本地属性", Meta = (DisplayName = "背景图"))
		UImage* BgImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "MES本地属性", Meta = (DisplayName = "颜色数组"))
		TArray<FColorArray> Colors;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MES本地属性", Meta = (DisplayName = "DontDestroy"))
		bool DontDestroy = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MES本地属性", Meta = (DisplayName = "UseInitPos"))
		bool UseInitPos = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "MES本地属性", Meta = (DisplayName = "表格数据"))
		FTableData TableData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "MES本地属性", Meta = (DisplayName = "CanConstruct"))
		bool CanConstruct = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "MES本地属性", Meta = (DisplayName = "MinSize"))
		FVector2D MinSize = FVector2D(10, 10);//用于拖动的时候 设定组件的最小大小用的

	UFUNCTION(BlueprintCallable, Category = "MESFunc")
		void SetComponetData(TArray<FString> SQLText);

	UFUNCTION(BlueprintCallable, Category = "MESFunc")
		void BindGetComponetData(FUpdateDataSingleDelegate CallBack);

	UFUNCTION(BlueprintCallable, Category = "MESFunc")
		void UnBindGetComponetData(FUpdateDataSingleDelegate CallBack);

	UFUNCTION(BlueprintCallable, Category = "MESFunc")
		void OnClick();

	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable, Category = "MESFunc")
		void DataUpdate();

	UFUNCTION(BlueprintCallable, Category = "MESFunc")
			UMESComponetBase* Clone();

	UFUNCTION(BlueprintCallable, Category = "MESFunc")
		static void BindSelectEvent(FSelectSingleDelegate SelectEvent);

	UFUNCTION(BlueprintCallable, Category = "MESFunc")
		static void UnBindSelectEvent(FSelectSingleDelegate SelectEvent);

	UFUNCTION(BlueprintCallable, Category = "MESFunc")
		void SetFinish();

	UFUNCTION(BlueprintCallable, Category = "MESFunc")
		void SetShow(bool isVisible);

	UFUNCTION(BlueprintCallable, Category = "MESFunc")
		bool GetIsSelected();
	
	UFUNCTION(BlueprintCallable, Category = "MESFunc")
		void GenUUID();

	UFUNCTION(BlueprintCallable, Category = "MESFunc")
		void SetPosAndSize(FVector2D Pos, FVector2D Size);

	UFUNCTION(BlueprintCallable, Category = "MESFunc")
		FColor GetColorByIndex(int index);

	UFUNCTION(BlueprintCallable, Category = "MESFunc")
		void SetBgAlpha(float alpha);

	UFUNCTION(BlueprintCallable, Category = "MESFunc")
		void SetDepth(int NewDepth);

	UFUNCTION(BlueprintCallable, Category = "MESFunc")
		void SetSelectColorIndex(int SelectColorIndex );

	UFUNCTION(BlueprintCallable, Category = "MESFunc")
		TArray<FString>	GetDataColoumn(FString FieldName);

	UFUNCTION(BlueprintCallable, Category = "MESFunc")
		TArray<FTableRowData> GetDataColoumns(FString FieldName);

	UFUNCTION(BlueprintCallable, Category = "MESFunc")
		void SetParent(int64 UUID);

	UFUNCTION(BlueprintCallable, Category = "MESFunc")
		UMESComponetBase* GetDontDestroyComponet(int StyleID);

	UFUNCTION(BlueprintCallable, Category = "MESFunc")
		void CopyPropertyFromAnother(UMESComponetBase* Left);

	UFUNCTION(BlueprintCallable, Category = "MESFunc")
		static void SetSelectUserWidget(UUserWidget* Widget);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MESFunc")
		int64 GetFilterRuleId();

	UFUNCTION(BlueprintCallable, Category = "MESFunc")
		UMESComponetData* ConvertMESComponetData();

	UFUNCTION(BlueprintCallable, Category = "MESFunc")
		 void SetMESComponetData(UMESComponetData* data);

	UFUNCTION(BlueprintCallable, Category = "MESUtil")
			static UMESComponetBase* GetUIComponetByUUID(int64 UUID);

	// make sure param of target funtion or event same like this:
	// param1:UObject* Sender
	// param2:FString Param
	UFUNCTION(BlueprintCallable, Category = "MESFunc")
		void ParseEvent(UObject* InnerObject,FString FunctionName,FString Param);

public :

	//~ Begin UWidget Interface
	virtual void SynchronizeProperties() override;
	virtual void PreSave(const class ITargetPlatform* TargetPlatform) override;
	//~ End UWidget Interface
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativePreConstruct() override;
	virtual void NativeDestruct() override;
protected:
	//~ Begin UWidget Interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
	//~ End UWidget Interface
	void OnObjectModified(UObject* Object);
public:
		static TMap<int, UMESComponetBase*> DontDestroyMap;
		bool PageChangeSave = true;
private:
	static TArray<UMESComponetBase*> ComponetArray;
	static FSelectComponetDelegate SelectComponetEvent;
	static UUserWidget* SelectWidget;
	bool IsSelected = false;
	bool IsForceDown = false;
	bool CanDrag = false;
	void GetComponetData();

	void HttpRequest(FString url,FString Methond);//当前仅仅支持 GET方式 后期会考虑 POST方式
	void OnRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	void HandleJson(FString Json);
	TArray<int> GetDataColoumnIndex(FString FieldName);

	FString GetFormBoundary();
	FString HttpCookie;

	FUpdateDataDelegate UpdateDataCallBack;
#if WITH_EDITOR
public:
	// UWidget interface
	virtual const FText GetPaletteCategory() override;
	// End UWidget interface
#endif

private:
	UFUNCTION()
		void OnDACColumnInfoResponse(FString JsonString);
	UFUNCTION()
		void OnDACTableDataResponse(FString JsonString);
};
