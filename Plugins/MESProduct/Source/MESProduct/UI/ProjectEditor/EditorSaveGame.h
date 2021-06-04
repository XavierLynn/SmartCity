// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RabbitMqProtocol.h"
#include "Misc/TextFilter.h"
#include "MESProduct/MESComponent/ComponetBase/MESComponetBase.h"
#include "MESProduct/MESComponent/ComponetBase/MESStyleBase.h"
#include "MESProduct/MESComponent/ComponetBase/MESActorBase.h"
#include "EditorSaveGame.generated.h"


DECLARE_DELEGATE_OneParam(FMQFindMenuCallback, FMQMenuData&);
DECLARE_DYNAMIC_DELEGATE(FPageComponentFilterCallback);


USTRUCT(BlueprintType)
struct FStyleInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "StyleID"))
	int StyleID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "StyleType"))
		FString StyleType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "StyleName"))
	FString StyleName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "CateGory"))
		FString CateGory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "SecondCateGory"))
		FString SecondCateGory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "PrefabPath"))
	FString PrefabPath;//预制路径

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "StylePath"))
	FString StylePath;//样式路径
};


USTRUCT(BlueprintType)
struct FSceneInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		int SceneID;

	UPROPERTY(BlueprintReadWrite)
		FString SceneName;

	UPROPERTY(BlueprintReadWrite)
		FString SceneSnapShot;

	UPROPERTY(BlueprintReadWrite)
		FString ScenePath;
	UPROPERTY(BlueprintReadWrite)
		bool CanCache;
};


/**
 * 
 */
UCLASS()
class MESPRODUCT_API AEditorSaveGame : public AActor//, public TSharedFromThis<AEditorSaveGame>
{
	GENERATED_BODY()

		typedef TTextFilter<const FMQMenuData& > MESMQMenuFilter;
	typedef TTextFilter<const UMESComponetBase* > MESPageComponentFilter;

public:
	// Sets default values for this actor's properties
	AEditorSaveGame();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
		
public:
	UPROPERTY(BlueprintReadWrite)
		TArray<FMQPageData> PageMap;
	UPROPERTY(BlueprintReadWrite)
		TArray<FMQMenuData> MenuMap;
	UPROPERTY(BlueprintReadWrite)
		TArray<FMQMenuData> FilterMenuMap;

	UPROPERTY(BlueprintReadOnly)
		TArray<UMESComponetBase*> ComponentsToAdd;
	UPROPERTY(BlueprintReadOnly)
		TArray<UMESComponetBase*> ComponentsToUpdate;
	UPROPERTY(BlueprintReadOnly)
		TArray<UMESComponetBase*> ComponentsToDelete;
	UPROPERTY(BlueprintReadWrite)
		TArray<FMQComponentData> ComponentMap;

	UPROPERTY(BlueprintReadOnly)
		TArray<AMESActorBase*> Actor3DToAdd;
	UPROPERTY(BlueprintReadOnly)
		TArray<AMESActorBase*> Actor3DToUpdate;
	UPROPERTY(BlueprintReadOnly)
		TArray<AMESActorBase*> Actor3DToDelete;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class USqliteTool* SqliteTool;

	UFUNCTION(BlueprintCallable)
		bool CreatePage(FMQPageData PageData, FMQMenuData MenuData);
	UFUNCTION(BlueprintCallable)
		bool IsExistPage(int InPageId);
	UFUNCTION(BlueprintCallable)
		FMQPageData GetPageData(int InPageId);

	UFUNCTION(BlueprintCallable)
		void SetPageData(FMQPageData PageData);

	UFUNCTION(BlueprintCallable)
		bool CopyPage(FMQMenuData MenuData);
	UFUNCTION(BlueprintCallable)
		bool DeletePage(FMQMenuData MenuData);
	UFUNCTION(BlueprintCallable)
		bool UpdateMenuName(int MenuID, FString NewName);

	UFUNCTION(BlueprintCallable)
		int GetNewPageId();
	UFUNCTION(BlueprintCallable)
		int GetMaxPageId();
	UFUNCTION(BlueprintCallable)
		FMQMenuData FindMenu(int MenuId, bool& IsFind);

	UFUNCTION(BlueprintCallable)
		static FString DateTimeToString(FDateTime DateTime);

	UFUNCTION(BlueprintCallable)
		bool LoadFromSqliteDB();

	UFUNCTION(BlueprintCallable)
		void SetTreeviewTextFilter(const FText& InFilterText);
	UFUNCTION(BlueprintImplementableEvent)
		void BP_OnTreeviewFilterChanged();

	UFUNCTION(BlueprintCallable)
		void SetPageComponentTextFilter(const FText& InFilterText, FPageComponentFilterCallback Callback);
	UFUNCTION(BlueprintCallable)
		bool CouldPassPageComponentFilter(UMESComponetBase* PageComponent);

	UFUNCTION(BlueprintCallable)
		bool SavePageAndComponents(int PageId);

	UFUNCTION(BlueprintCallable)
		void GetInitPos(int PageId,FVector& Pos, FRotator& Rotate, FString& LevelSequencePath, bool& GetSucess);

	UFUNCTION(BlueprintCallable)
		void SetInitPos(int PageId,FVector Pos, FRotator Rotate, FString LevelSequencePath);

	UFUNCTION(BlueprintCallable)
		bool LoadComponents(int PageId);
	UFUNCTION(BlueprintCallable)
		void MarkNewComponent(UMESComponetBase* NewComponent);
	UFUNCTION(BlueprintCallable)
		void MarkDeleteComponent(UMESComponetBase* DeleteComponent);
	// Used in online project after SAVING projects to ensure local data correctness.
	UFUNCTION(BlueprintCallable)
		void RefreshMarkOfComponent();
	// Only used in offline project for REDO ADD or UNDO DELETE operation.
	UFUNCTION(BlueprintCallable)
		void RemarkNewComponent(UMESComponetBase* NewComponent);
	UFUNCTION(BlueprintCallable)
		void LoadComponentsServer(int PageId);
	UFUNCTION(BlueprintCallable)
		void FindComponentsByPageId(int PageId, TArray<FMQComponentData>& OutTargetComponents);


	UFUNCTION(BlueprintCallable, Category = "MESUtil")
		UMESStyleBase* LoadStyle(FString StylePath);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MES组件属性", Meta = (DisplayName = "StyleMap"))
		TMap<int, FStyleInfo> ComponentStyleMap;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MES组件属性", Meta = (DisplayName = "SceneMap"))
		TMap<int, FSceneInfo> SceneMap;


	UFUNCTION(BlueprintCallable)
		void MarkNewActor3D(AMESActorBase* NewActor3D);
	UFUNCTION(BlueprintCallable)
		void MarkDeleteActor3D(AMESActorBase* DeleteActor3D);

	UFUNCTION(BlueprintCallable)
		static FString GetStylePathFromPrefab (FString PrefabPath);

	UFUNCTION(BlueprintCallable)
		static UClass* LoadClassFromPath(UObject* Outer,FString blueprintPath);

	UFUNCTION(BlueprintCallable)
		static UObject* LoadObjectFromPath(UObject* Outer, FString blueprintPath);
private:

	void FindMenuAndDo(FMQMenuData& Parent, int TargetMenuId, FMQFindMenuCallback Callback, bool& IsFind);

	// datatable names
	const FString PAGE_NAME = "SDC_PAGE";
	const FString MENU_NAME = "SDC_MENU";
	const FString COMPONENT_NAME = "SDC_COMPONENT";
	const FString STYLE_NAME = "SDC_STYLE";
	const FString PAGE_COMPONENT_NAME = "SDC_PAGE_COMPONENT";
	const FString SCENE_NAME = "SDC_MES_SCENE";

	void LoadPageData(struct FSqliteData& SqliteData);
	void LoadMenuData(struct FSqliteData& SqliteData);

	TSharedPtr<MESMQMenuFilter> TreeViewFilter;
	void OnTreeviewFilterChanged();
	void MenuDataToString(const FMQMenuData& MenuData, TArray<FString>& OutSearchStrings);

	TSharedPtr< MESPageComponentFilter> PageComponentFilter;
	FPageComponentFilterCallback PageComponentFilterCallback;
	void OnPageComponentFilterChanged();
	void PageComponentToString(const UMESComponetBase* PageComponent, TArray<FString>& OutSearchStrings);

	// OutArrayIndex, 0:BaseInfo, 1:StyleInfo, 2:QueryRule, 3:StaticData, 4:EventInfo
	UFUNCTION(BlueprintCallable, BlueprintPure)
	void GetComponentInfo(const UMESComponetBase* Component, TArray<FString>& OutInfoJsonStrings);
	void SetComponentInfo(UMESComponetBase* Component, const TArray<FString>& InfoJsonStrings);

	// OutArrayIndex, 0:BaseInfo, 1:StyleInfo, 2:QueryRule, 3:StaticData, 4:EventInfo
	UFUNCTION(BlueprintCallable, BlueprintPure)
	void GetActor3DInfo(const AMESActorBase* Component, TArray<FString>& OutInfoJsonStrings);
	void SetActor3DInfo(AMESActorBase* Component, const TArray<FString>& InfoJsonStrings);
	private:
		TMap<int64, UMESComponetBase*> TempMESDic = TMap<int64, UMESComponetBase*>();
};
