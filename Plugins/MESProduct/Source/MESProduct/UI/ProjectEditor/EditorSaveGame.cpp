// Fill out your copyright notice in the Description page of Project Settings.


#include "EditorSaveGame.h"
#include "MESProduct/MESComponent/Sqlite/SqliteTool.h"
#include "MESProduct/Tool/MESPathUtility.h"
#include "MESProduct/Tool/MESGameInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "MESProduct/Tool/MESBlueprintFunctionLibrary.h"
#include "MESProduct/MESComponent/OperationHistory/MESOperationHistory.h"
#include "Engine\Classes\Kismet\GameplayStatics.h"
// Sets default values
AEditorSaveGame::AEditorSaveGame()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SqliteTool = CreateDefaultSubobject<USqliteTool>(TEXT("SqliteTool"));
}

// Called when the game starts or when spawned
void AEditorSaveGame::BeginPlay()
{
	Super::BeginPlay();

	// treeview filter initialize
	MESMQMenuFilter::FItemToStringArray TreeviewItemToStringDelegate;
	TreeviewItemToStringDelegate.BindUObject(this, &AEditorSaveGame::MenuDataToString);
	TreeViewFilter = MakeShareable(new MESMQMenuFilter(TreeviewItemToStringDelegate));
	TreeViewFilter->OnChanged().AddUObject(this, &AEditorSaveGame::OnTreeviewFilterChanged);

	// page component filter initialize
	MESPageComponentFilter::FItemToStringArray PageComponentItemToStringDelegate;
	PageComponentItemToStringDelegate.BindUObject(this, &AEditorSaveGame::PageComponentToString);
	PageComponentFilter = MakeShareable(new MESPageComponentFilter(PageComponentItemToStringDelegate));
	PageComponentFilter->OnChanged().AddUObject(this, &AEditorSaveGame::OnPageComponentFilterChanged);

	UMESGameInstance* GameInstance = UMESGameInstance::Get(GetWorld());
	if (GameInstance)
	{
		FString DBPath;
		if (GameInstance->IsProjectOffline)
		{
			DBPath = UMESPathUtility::GetStandaloneProjectDBPath(GameInstance->ProjectInfo_Offline.ProjectName);
		}
		else
		{
			DBPath = UMESPathUtility::GetServerProjectDBPath(GameInstance->ProjectInfo_Server.projectName);
		}

		// load style data
		FString SelectStyleSql = "select * from " + STYLE_NAME;
		FTableData StyleData;
		USqliteTool::LoadTableData(DBPath, SelectStyleSql, STYLE_NAME, StyleData);
		for (FTableRowData StyleRowData : StyleData.RowDatas)
		{
			if (StyleRowData.ColumnDatas.Num() < 6)
			{
				continue;
			}
			FStyleInfo info;
			int StyleId = FCString::Atoi(*StyleRowData.ColumnDatas[0]);
			info.StyleID = StyleId;
			info.StyleName = StyleRowData.ColumnDatas[1];
			info.CateGory = StyleRowData.ColumnDatas[2];
			info.SecondCateGory = StyleRowData.ColumnDatas[3];
			info.PrefabPath = StyleRowData.ColumnDatas[4].Left(StyleRowData.ColumnDatas[4].Len() - 1) + TEXT("_C'");
			info.StylePath = GetStylePathFromPrefab(StyleRowData.ColumnDatas[4]);
			info.StyleType = StyleRowData.ColumnDatas[5];
			ComponentStyleMap.Add(StyleId, info);
		}

		// load scene/level data
		FString SelectSceneSql = "select * from " + SCENE_NAME;
		FTableData SceneData;
		USqliteTool::LoadTableData(DBPath, SelectSceneSql, SCENE_NAME, SceneData);
		for (FTableRowData SceneRowData : SceneData.RowDatas)
		{
			if (SceneRowData.ColumnDatas.Num() < 4)
			{
				continue;
			}
			FSceneInfo info;
			int SceneId = FCString::Atoi(*SceneRowData.ColumnDatas[0]);
			info.SceneID = SceneId;
			info.SceneName = SceneRowData.ColumnDatas[1];
			info.SceneSnapShot = SceneRowData.ColumnDatas[2];
			info.ScenePath = SceneRowData.ColumnDatas[3];
			info.CanCache = (SceneRowData.ColumnDatas[4] == TEXT("1"));
			SceneMap.Add(SceneId, info);
		}
	}
}

// Called every frame
void AEditorSaveGame::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AEditorSaveGame::CreatePage(FMQPageData PageData, FMQMenuData MenuData)
{
	int PageId = PageData.baseInfo.pageId;
	if (IsExistPage(PageId))
	{
		return false;
	}

	// Page and Menu is one-to-one correspondence, pageId = menuId
	UMESGameInstance* GameInstance = UMESGameInstance::Get(GetWorld());
	if (GameInstance && SqliteTool)
	{
		// save page and menu to sqlite Db. Use insert statement into table SDC_PAGE and SDC_MENU.
		FString DBPath = UMESPathUtility::GetStandaloneProjectDBPath(GameInstance->ProjectInfo_Offline.ProjectName);

		TArray<FString> InsertStringList;
		FString PageInsertSql = "insert into " + PAGE_NAME + " values (";
		PageInsertSql += "'" + FString::FromInt(PageData.baseInfo.pageId) + "',";
		PageInsertSql += "'" + FString::FromInt(PageData.baseInfo.menuId) + "',";
		PageInsertSql += "'" + FString::FromInt(PageData.baseInfo.sceneId) + "',";
		PageInsertSql += "'" + FString::FromInt(PageData.baseInfo.uiMaskImage) + "',";
		PageInsertSql += "'" + PageData.baseInfo.pageAnimation + "',";
		PageInsertSql += "'" + FString::FromInt(PageData.baseInfo.pageSkyBox) + "',";
		PageInsertSql += "'" + PageData.baseInfo.environmentParam + "',";
		PageInsertSql += "'" + FString::FromInt(PageData.baseInfo.isPublish) + "',";
		PageInsertSql += "'" + PageData.baseInfo.createUser + "',";
		PageInsertSql += "'" + PageData.baseInfo.createTime + "',";
		PageInsertSql += "'" + PageData.baseInfo.modifyUser + "',";
		PageInsertSql += "'" + PageData.baseInfo.modifyTime + "')";
		InsertStringList.Add(PageInsertSql);

		FString MenuInsertSql = "insert into " + MENU_NAME + " values (";
		MenuInsertSql += "'" + FString::FromInt(MenuData.menuId) + "',";
		MenuInsertSql += "'" + MenuData.menuName + "',";
		MenuInsertSql += "'" + FString::FromInt(MenuData.parentId) + "',";
		MenuInsertSql += "'" + FString::FromInt(MenuData.MenuLevel) + "',";
		MenuInsertSql += "'" + FString::FromInt(MenuData.isLeaf) + "',";
		MenuInsertSql += "'" + PageData.baseInfo.createUser + "',";
		MenuInsertSql += "'" + PageData.baseInfo.createTime + "',";
		MenuInsertSql += "'" + PageData.baseInfo.modifyUser + "',";
		MenuInsertSql += "'" + PageData.baseInfo.modifyTime + "')";
		InsertStringList.Add(MenuInsertSql);

		if (SqliteTool->ExecuteSqlString(DBPath, InsertStringList))
		{
			// maintain our local data, add new page and menu.
			PageMap.Add(PageData);

			int MenuParentId = MenuData.parentId;
			bool IsFindParent = false;
			FMQFindMenuCallback Callback;
			Callback.BindLambda([&](FMQMenuData& TargetMenu) {
				TargetMenu.ChildMenuList.Add(MenuData);
				TargetMenu.isLeaf = false;

				TArray<FString> UpdateSqlList;
				FString UpdateMenuSql = "update " + MENU_NAME + " set IS_LEAF = false where LIST_ID = " + FString::FromInt(TargetMenu.menuId);
				UpdateSqlList.Add(UpdateMenuSql);

				SqliteTool->ExecuteSqlString(DBPath, UpdateSqlList);
			});
			for (FMQMenuData& Menu : MenuMap)
			{
				FindMenuAndDo(Menu, MenuParentId, Callback, IsFindParent);
				if (IsFindParent)
				{
					break;
				}
			}
			if (!IsFindParent)
			{
				MenuData.MenuLevel = 0;
				MenuMap.Add(MenuData);
			}
			return true;
		}
	}

	return false;
}

bool AEditorSaveGame::IsExistPage(int InPageId)
{
	for (FMQPageData PageData : PageMap)
	{
		int PageId = PageData.baseInfo.pageId;
		if (PageId == InPageId)
		{
			return true;
		}
	}

	return false;
}

FMQPageData AEditorSaveGame::GetPageData(int InPageId)
{
	for (FMQPageData PageData : PageMap)
	{
		int PageId = PageData.baseInfo.pageId;
		if (PageId == InPageId)
		{
			return PageData;
		}
	}

	return FMQPageData();
}

void AEditorSaveGame::SetPageData(FMQPageData PageData)
{
	for (int i = 0;i< PageMap.Num();i++)
	{
		if (PageMap[i].baseInfo.pageId == PageData.baseInfo.pageId)
		{
			PageMap[i] = PageData;
			break;
		}
	}
}

bool AEditorSaveGame::CopyPage(FMQMenuData MenuData)
{
	if (!MenuData.isLeaf)
	{
		UE_LOG(LogTemp, Warning, TEXT("copy page failed, page is not leaf"));
		return false;
	}

	UMESGameInstance* GameInstance = UMESGameInstance::Get(GetWorld());
	if (GameInstance)
	{
		FMQPageData NewPageData = GetPageData(MenuData.menuId);
		FMQMenuData NewMenuData = MenuData;

		int NewId = GetNewPageId();
		FString NowTime = FDateTime::Now().ToString();
		NewPageData.baseInfo.pageId = NewId;
		NewPageData.baseInfo.menuId = NewId;
		NewPageData.baseInfo.createUser = GameInstance->UserName;
		NewPageData.baseInfo.createTime = NowTime;

		NewMenuData.menuId = NewId;
		NewMenuData.menuName = TEXT("副本") + NewMenuData.menuName + NowTime;

		return CreatePage(NewPageData, NewMenuData);
	}
	return false;
}

bool AEditorSaveGame::DeletePage(FMQMenuData MenuData)
{
	if (!MenuData.isLeaf)
	{
		UE_LOG(LogTemp, Warning, TEXT("delete page failed, page is not leaf"));
		return false;
	}

	UMESGameInstance* GameInstance = UMESGameInstance::Get(GetWorld());
	if (GameInstance && SqliteTool)
	{
		// remove page and menu from sqlite Db.
		FString DBPath = UMESPathUtility::GetStandaloneProjectDBPath(GameInstance->ProjectInfo_Offline.ProjectName);

		TArray<FString> DeleteSqlList;
		FString DeletePageSql = "delete from " + PAGE_NAME + " where LIST_ID = " + FString::FromInt(MenuData.menuId);
		DeleteSqlList.Add(DeletePageSql);
		FString DeleteMenuSql = "delete from " + MENU_NAME + " where LIST_ID = " + FString::FromInt(MenuData.menuId);
		DeleteSqlList.Add(DeleteMenuSql);
		FString DeleteComponentSql = "delete from " + COMPONENT_NAME + " where LIST_ID in (select COMPONENT_ID from " + PAGE_COMPONENT_NAME + " where PAGE_ID = " + FString::FromInt(MenuData.menuId) + ")";
		DeleteSqlList.Add(DeleteComponentSql);
		FString DeletePageComponentSql = "delete from " + PAGE_COMPONENT_NAME + " where PAGE_ID = " + FString::FromInt(MenuData.menuId);
		DeleteSqlList.Add(DeletePageComponentSql);

		if (SqliteTool->ExecuteSqlString(DBPath, DeleteSqlList))
		{
			// remove local menu
			int PageId = MenuData.menuId;
			int ParentId = MenuData.parentId;
			FMQFindMenuCallback Callback;
			Callback.BindLambda([&](FMQMenuData& TargetMenu) {
				for (int i = 0; i < TargetMenu.ChildMenuList.Num(); ++i)
				{
					if (TargetMenu.ChildMenuList[i].menuId == PageId)
					{
						if (TargetMenu.ChildMenuList.Num() == 1)
						{
							TargetMenu.isLeaf = true;

							TArray<FString> UpdateSqlList;
							FString UpdateMenuSql = "update " + MENU_NAME + " set IS_LEAF = true where LIST_ID = " + FString::FromInt(TargetMenu.menuId);
							UpdateSqlList.Add(UpdateMenuSql);

							SqliteTool->ExecuteSqlString(DBPath, UpdateSqlList);
						}
						TargetMenu.ChildMenuList.RemoveAt(i);
						break;
					}
				}
			});
			bool IsFind = false;
			for (int i = 0; i < MenuMap.Num(); ++i)
			{
				FMQMenuData& Menu = MenuMap[i];
				if (Menu.menuId == PageId)
				{
					MenuMap.RemoveAt(i);
					IsFind = true;
					break;
				}
				FindMenuAndDo(Menu, ParentId, Callback, IsFind);
				if (IsFind)
				{
					break;
				}
			}
			// remove local page
			for (int i = 0; i < PageMap.Num(); ++i)
			{
				if (PageId == PageMap[i].baseInfo.pageId)
				{
					PageMap.RemoveAt(i);
					break;
				}
			}
			return IsFind;
		}
	}

	return false;
}

bool AEditorSaveGame::UpdateMenuName(int MenuID, FString NewName)
{
	UMESGameInstance* GameInstance = UMESGameInstance::Get(GetWorld());
	if (GameInstance && SqliteTool)
	{
		FString DBPath = UMESPathUtility::GetStandaloneProjectDBPath(GameInstance->ProjectInfo_Offline.ProjectName);

		TArray<FString> UpdateSqlList;
		FString UpdateMenuSql = "update " + MENU_NAME + " set MENU_NAME = '" + NewName + "' where LIST_ID = " + FString::FromInt(MenuID);
		UpdateSqlList.Add(UpdateMenuSql);

		if (SqliteTool->ExecuteSqlString(DBPath, UpdateSqlList))
		{
			FMQFindMenuCallback Callback;
			Callback.BindLambda([&](FMQMenuData& TargetMenu) {
				TargetMenu.menuName = NewName;
			});

			bool IsFind;
			for (FMQMenuData& MenuData : MenuMap)
			{
				FindMenuAndDo(MenuData, MenuID, Callback, IsFind);
				if (IsFind)
				{
					break;
				}
			}
		}
	}
	return false;
}

int AEditorSaveGame::GetNewPageId()
{
	return GetMaxPageId() + 1;
}

int AEditorSaveGame::GetMaxPageId()
{
	int MaxPageId = 0;
	for (FMQPageData PageData : PageMap)
	{
		int PageId = PageData.baseInfo.pageId;
		if (MaxPageId < PageId)
		{
			MaxPageId = PageId;
		}
	}
	return MaxPageId;
}

FMQMenuData AEditorSaveGame::FindMenu(int MenuId, bool & IsFind)
{
	FMQMenuData Result;
	FMQFindMenuCallback Callback;
	Callback.BindLambda([&](FMQMenuData& TargetMenu) {
		Result = TargetMenu;
	});

	for (FMQMenuData& MenuData : MenuMap)
	{
		FindMenuAndDo(MenuData, MenuId, Callback, IsFind);
		if (IsFind)
		{
			break;
		}
	}
	return Result;
}

FString AEditorSaveGame::DateTimeToString(FDateTime DateTime)
{
	return DateTime.ToString();
}

bool AEditorSaveGame::LoadFromSqliteDB()
{
	UMESGameInstance* GameInstance = UMESGameInstance::Get(GetWorld());
	if (GameInstance && SqliteTool)
	{
		FString DBPath = UMESPathUtility::GetStandaloneProjectDBPath(GameInstance->ProjectInfo_Offline.ProjectName);
		FSqliteData PageSqliteData = SqliteTool->LoadLocalData(DBPath, PAGE_NAME);
		LoadPageData(PageSqliteData);

		FSqliteData MenuSqliteData = SqliteTool->LoadLocalData(DBPath, MENU_NAME);
		LoadMenuData(MenuSqliteData);

		return true;
	}

	return false;
}

void AEditorSaveGame::SetTreeviewTextFilter(const FText& InFilterText)
{
	TreeViewFilter->SetRawFilterText(InFilterText);
}

void AEditorSaveGame::SetPageComponentTextFilter(const FText & InFilterText, FPageComponentFilterCallback Callback)
{
	PageComponentFilterCallback.Unbind();
	PageComponentFilterCallback = Callback;
	PageComponentFilter->SetRawFilterText(InFilterText);
}

bool AEditorSaveGame::CouldPassPageComponentFilter(UMESComponetBase * PageComponent)
{
	return PageComponentFilter->PassesFilter(PageComponent);
}

bool AEditorSaveGame::SavePageAndComponents(int PageId)
{
	//AMESActorBase will be treat as a page component in Sqlite Db, include APointActorBase.
	UMESGameInstance* GameInstance = UMESGameInstance::Get(GetWorld());
	if (GameInstance && SqliteTool)
	{
		FString DBPath = UMESPathUtility::GetStandaloneProjectDBPath(GameInstance->ProjectInfo_Offline.ProjectName);

		FString PageIdStr = FString::FromInt(PageId);
		FString SelectPageSql = "select * from " + PAGE_NAME + " where LIST_ID = " + PageIdStr;
		FTableData PageData;
		USqliteTool::LoadTableData(DBPath, SelectPageSql, PAGE_NAME, PageData);

		if (PageData.RowDatas.Num() <= 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("save page and components failed, no such page id: %d"), *PageIdStr);
			return false;
		}

		// save page.baseInfo
		FMQPageData MqPageData = GetPageData(PageId);
		TArray<FString> UpdatePageSqlList;
		FString UpdatePageSql = "update " + PAGE_NAME + " set ";
		UpdatePageSql += " SCENE_ID = '" + FString::FromInt(MqPageData.baseInfo.sceneId) + "',";
		UpdatePageSql += " UI_MASK_IMAGE = '" + FString::FromInt(MqPageData.baseInfo.uiMaskImage) + "',";

		FString PageAnimation = MqPageData.baseInfo.pageAnimation.Replace(TEXT("'"), TEXT("''"));
		UpdatePageSql += " PAGE_ANIMATION = '" + PageAnimation + "',";
		UpdatePageSql += " PAGE_SKY_BOX = '" + FString::FromInt(MqPageData.baseInfo.pageSkyBox) + "',";
		UpdatePageSql += " ENVIRONMENT_PARAM = '" + MqPageData.baseInfo.environmentParam + "',";
		UpdatePageSql += " IS_PUBLISH = '" + FString::FromInt(MqPageData.baseInfo.isPublish) + "',";
		UpdatePageSql += " MODIFY_USER_ID = '" + GameInstance->UserName + "',";
		UpdatePageSql += " MODIFY_TIME = '" + FDateTime::Now().ToString() + "'";
		UpdatePageSql += " where LIST_ID = " + FString::FromInt(PageId);
		UpdatePageSqlList.Add(UpdatePageSql);

		if (UpdatePageSqlList.Num() > 0 && !SqliteTool->ExecuteSqlString(DBPath, UpdatePageSqlList))
		{
			UE_LOG(LogTemp, Warning, TEXT("update page failed"));
			return false;
		}

		// delete component
		TArray<FString> DeleteSqlList;
		for (UMESComponetBase* DeleteComponent : ComponentsToDelete)
		{
			FString UUIDStr;
			UMESBlueprintFunctionLibrary::Int64ToString(DeleteComponent->CommonData.UUID, UUIDStr);

			FString DeleteComponentSql = "delete from " + COMPONENT_NAME + " where LIST_ID = " + UUIDStr;
			DeleteSqlList.Add(DeleteComponentSql);
			FString DeletePageComponentSql = "delete from " + PAGE_COMPONENT_NAME + " where PAGE_ID = " + PageIdStr + " and COMPONENT_ID = " + UUIDStr;
			DeleteSqlList.Add(DeletePageComponentSql);
		}
		for (AMESActorBase* DeleteActor3D : Actor3DToDelete)
		{
			FString UUIDStr;
			UMESBlueprintFunctionLibrary::Int64ToString(DeleteActor3D->CommonData.UUID, UUIDStr);

			FString DeleteComponentSql = "delete from " + COMPONENT_NAME + " where LIST_ID = " + UUIDStr;
			DeleteSqlList.Add(DeleteComponentSql);
			FString DeletePageComponentSql = "delete from " + PAGE_COMPONENT_NAME + " where PAGE_ID = " + PageIdStr + " and COMPONENT_ID = " + UUIDStr;
			DeleteSqlList.Add(DeletePageComponentSql);
		}

		if (DeleteSqlList.Num() > 0)
		{
			if (SqliteTool->ExecuteSqlString(DBPath, DeleteSqlList))
			{
				ComponentsToDelete.Empty();
				Actor3DToDelete.Empty();
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("delete Components failed"));
			}
		}

		// update component
		TArray<FString> UpdateSqlList;
		for (UMESComponetBase* UpdateComponent : ComponentsToUpdate)
		{
			TArray<FString> ComponentInfoList;
			GetComponentInfo(UpdateComponent, ComponentInfoList);

			FString UUIDStr;
			UMESBlueprintFunctionLibrary::Int64ToString(UpdateComponent->CommonData.UUID, UUIDStr);

			FString UpdateComponentSql = "update " + COMPONENT_NAME + " set ";
			UpdateComponentSql += "BASE_INFO = '" + ComponentInfoList[0] + "',";
			UpdateComponentSql += "STYLE_INFO = '" + ComponentInfoList[1] + "',";
			UpdateComponentSql += "QUERY_RULE = '" + ComponentInfoList[2] + "',";
			UpdateComponentSql += "STATIC_DATA = '" + ComponentInfoList[3] + "',";
			UpdateComponentSql += "EVENT_INFO = '" + ComponentInfoList[4] + "',";
			UpdateComponentSql += "MODIFY_USER_ID = '" + GameInstance->UserName + "',";
			UpdateComponentSql += "MODIFY_TIME = '" + FDateTime::Now().ToString() + "'";
			UpdateComponentSql += " where LIST_ID = " + UUIDStr;
			UpdateSqlList.Add(UpdateComponentSql);
		}
		for (AMESActorBase* UpdateActor3D : Actor3DToUpdate)
		{
			TArray<FString> ComponentInfoList;
			GetActor3DInfo(UpdateActor3D, ComponentInfoList);

			FString UUIDStr;
			UMESBlueprintFunctionLibrary::Int64ToString(UpdateActor3D->CommonData.UUID, UUIDStr);

			FString UpdateComponentSql = "update " + COMPONENT_NAME + " set ";
			UpdateComponentSql += "BASE_INFO = '" + ComponentInfoList[0] + "',";
			UpdateComponentSql += "STYLE_INFO = '" + ComponentInfoList[1] + "',";
			UpdateComponentSql += "QUERY_RULE = '" + ComponentInfoList[2] + "',";
			UpdateComponentSql += "STATIC_DATA = '" + ComponentInfoList[3] + "',";
			UpdateComponentSql += "EVENT_INFO = '" + ComponentInfoList[4] + "',";
			UpdateComponentSql += "MODIFY_USER_ID = '" + GameInstance->UserName + "',";
			UpdateComponentSql += "MODIFY_TIME = '" + FDateTime::Now().ToString() + "'";
			UpdateComponentSql += " where LIST_ID = " + UUIDStr;
			UpdateSqlList.Add(UpdateComponentSql);
		}

		if (UpdateSqlList.Num() > 0 && !SqliteTool->ExecuteSqlString(DBPath, UpdateSqlList))
		{
			UE_LOG(LogTemp, Warning, TEXT("update Components failed"));
		}

		// add component
		if ( (ComponentsToAdd.Num() > 0) || (Actor3DToAdd.Num() > 0) )
		{
			TArray<FString> InsertStringList;
			FString ComponentInsertSql = "insert into " + COMPONENT_NAME + " values ";
			FString PageComponentInsertSql = "insert into " + PAGE_COMPONENT_NAME + " (PAGE_ID, COMPONENT_ID, CREATE_USER_ID, CREATE_TIME) values ";
			for (int i = 0; i < ComponentsToAdd.Num(); ++i)
			{
				UMESComponetBase* InsertComponent = ComponentsToAdd[i];
				TArray<FString> ComponentInfoList;
				GetComponentInfo(InsertComponent, ComponentInfoList);

				FString UUIDStr;
				UMESBlueprintFunctionLibrary::Int64ToString(InsertComponent->CommonData.UUID, UUIDStr);

				ComponentInsertSql += "(";
				ComponentInsertSql += "'" + UUIDStr + "',";// list id
				ComponentInsertSql += "'-1',";// parent id
				ComponentInsertSql += "'0',";// component type
				ComponentInsertSql += "'" + ComponentInfoList[0] + "',";// base info
				ComponentInsertSql += "'" + ComponentInfoList[1] + "',";// style info
				ComponentInsertSql += "'" + ComponentInfoList[2] + "',";// query rule
				ComponentInsertSql += "'" + ComponentInfoList[3] + "',";// static data
				ComponentInsertSql += "'" + ComponentInfoList[4] + "',";// event info
				ComponentInsertSql += "'" + GameInstance->UserName + "',";// create user
				ComponentInsertSql += "'" + FDateTime::Now().ToString() + "',";// create time
				ComponentInsertSql += "'',";// modify user
				ComponentInsertSql += "''";// modify time

				PageComponentInsertSql += "(";
				PageComponentInsertSql += "'" + PageIdStr + "',";// page id
				PageComponentInsertSql += "'" + UUIDStr + "',";// component id
				//PageComponentInsertSql += "'0',";// component flag
				//PageComponentInsertSql += "'0',";// display level
				//PageComponentInsertSql += "'0',";// is hide in aem
				//PageComponentInsertSql += "'0',";// is hide in sre
				PageComponentInsertSql += "'" + GameInstance->UserName + "',";// create user
				PageComponentInsertSql += "'" + FDateTime::Now().ToString() + "'";// create time
				//PageComponentInsertSql += "'',";// modify user
				//PageComponentInsertSql += "''";// modify time

				if (i < ComponentsToAdd.Num() - 1 || Actor3DToAdd.Num() > 0)
				{
					ComponentInsertSql += "),";
					PageComponentInsertSql += "),";
				}
				else
				{
					ComponentInsertSql += ")";
					PageComponentInsertSql += ")";
				}
			}
			for (int i = 0; i < Actor3DToAdd.Num(); ++i)
			{
				AMESActorBase* InsertActor3D = Actor3DToAdd[i];
				TArray<FString> ComponentInfoList;
				GetActor3DInfo(InsertActor3D, ComponentInfoList);

				FString UUIDStr;
				UMESBlueprintFunctionLibrary::Int64ToString(InsertActor3D->CommonData.UUID, UUIDStr);

				ComponentInsertSql += "(";
				ComponentInsertSql += "'" + UUIDStr + "',";// list id
				ComponentInsertSql += "'-1',";// parent id
				ComponentInsertSql += "'0',";// component type
				ComponentInsertSql += "'" + ComponentInfoList[0] + "',";// base info
				ComponentInsertSql += "'" + ComponentInfoList[1] + "',";// style info
				ComponentInsertSql += "'" + ComponentInfoList[2] + "',";// query rule
				ComponentInsertSql += "'" + ComponentInfoList[3] + "',";// static data
				ComponentInsertSql += "'" + ComponentInfoList[4] + "',";// event info
				ComponentInsertSql += "'" + GameInstance->UserName + "',";// create user
				ComponentInsertSql += "'" + FDateTime::Now().ToString() + "',";// create time
				ComponentInsertSql += "'',";// modify user
				ComponentInsertSql += "''";// modify time

				PageComponentInsertSql += "(";
				PageComponentInsertSql += "'" + PageIdStr + "',";// page id
				PageComponentInsertSql += "'" + UUIDStr + "',";// component id
				//PageComponentInsertSql += "'0',";// component flag
				//PageComponentInsertSql += "'0',";// display level
				//PageComponentInsertSql += "'0',";// is hide in aem
				//PageComponentInsertSql += "'0',";// is hide in sre
				PageComponentInsertSql += "'" + GameInstance->UserName + "',";// create user
				PageComponentInsertSql += "'" + FDateTime::Now().ToString() + "'";// create time
				//PageComponentInsertSql += "'',";// modify user
				//PageComponentInsertSql += "''";// modify time

				if (i < Actor3DToAdd.Num() - 1)
				{
					ComponentInsertSql += "),";
					PageComponentInsertSql += "),";
				}
				else
				{
					ComponentInsertSql += ")";
					PageComponentInsertSql += ")";
				}
			}

			InsertStringList.Add(ComponentInsertSql);
			InsertStringList.Add(PageComponentInsertSql);

			if (SqliteTool->ExecuteSqlString(DBPath, InsertStringList))
			{
				ComponentsToUpdate.Append(ComponentsToAdd);
				ComponentsToAdd.Empty();

				Actor3DToUpdate.Append(Actor3DToAdd);
				Actor3DToAdd.Empty();
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("add Components failed"));
			}
		}
		
		UMESOperationHistory::Get()->ClearOperation();
		UE_LOG(LogTemp, Warning, TEXT("Save Components Over"));
		return true;
	}

	return false;
}

void AEditorSaveGame::GetInitPos(int PageId, FVector& Pos, FRotator& Rotate, FString& LevelSequencePath,bool& GetSucess)
{
	  GetPageData(PageId);
	  FMQPageData pageData = GetPageData(PageId);
	  FString S = pageData.baseInfo.pageAnimation;
	  TArray<FString> result;
	  pageData.baseInfo.pageAnimation.ParseIntoArray(result, TEXT("|"));
	  if (result.Num() < 6)
	  {
		  GetSucess = false;
	  }
	  else
	  {
		  GetSucess = true;
		  Pos.X = FCString::Atof(*result[0]);
		  Pos.Y = FCString::Atof(*result[1]);
		  Pos.Z = FCString::Atof(*result[2]);
		  Rotate.Pitch = FCString::Atof(*result[3]);
		  Rotate.Roll = FCString::Atof(*result[4]);
		  Rotate.Yaw = FCString::Atof(*result[5]);

		  if (result.Num() >= 7)
		  {
			  LevelSequencePath = result[6];
		  }
		  else
		  {
			  LevelSequencePath = "";
		  }
	  }

}

void AEditorSaveGame::SetInitPos(int PageId, FVector Pos, FRotator Rotate, FString LevelSequencePath)
{
	FMQPageData pageData = GetPageData(PageId);
	pageData.baseInfo.pageAnimation = FString::SanitizeFloat(Pos.X) + TEXT("|") + FString::SanitizeFloat(Pos.Y) + TEXT("|") + FString::SanitizeFloat(Pos.Z) + TEXT("|") +
		FString::SanitizeFloat(Rotate.Pitch) + TEXT("|") + FString::SanitizeFloat(Rotate.Roll) + TEXT("|") + FString::SanitizeFloat(Rotate.Yaw) +
		+TEXT("|") + LevelSequencePath;
	SetPageData(pageData);
}

bool AEditorSaveGame::LoadComponents(int PageId)
{
	UMESGameInstance* GameInstance = UMESGameInstance::Get(GetWorld());
	if (GameInstance && SqliteTool)
	{
		ComponentsToAdd.Empty();
		ComponentsToDelete.Empty();
		ComponentsToUpdate.Empty();
		for (auto Iter = UMESComponetBase::DontDestroyMap.CreateIterator(); Iter; ++Iter)
		{
			UMESComponetBase* a = Iter->Value;
			a->PageChangeSave = false;
		}
		Actor3DToAdd.Empty();
		Actor3DToDelete.Empty();
		Actor3DToUpdate.Empty();

		FString DBPath = UMESPathUtility::GetStandaloneProjectDBPath(GameInstance->ProjectInfo_Offline.ProjectName);

		// load component data
		FString SelectComponentSql = "select a.* from " + COMPONENT_NAME + " as a, " + PAGE_COMPONENT_NAME + " as b";
		SelectComponentSql += " where a.LIST_ID = b.COMPONENT_ID and b.PAGE_ID = " + FString::FromInt(PageId);
		FTableData ComponentData;
		USqliteTool::LoadTableData(DBPath, SelectComponentSql, "", ComponentData);
		TempMESDic.Empty();
		for (FTableRowData RowData : ComponentData.RowDatas)
		{
			if (RowData.ColumnDatas.Num() < 12)
			{
				UE_LOG(LogTemp, Warning, TEXT("load components failed, component data error, continue"));
				continue;
			}

			const int64 ComponentId = FCString::Atoi64(*RowData.ColumnDatas[0]);
			const FString BaseInfo = RowData.ColumnDatas[3];
			const FString StyleInfo = RowData.ColumnDatas[4];
			const FString QueryRule = RowData.ColumnDatas[5];
			const FString StaticData = RowData.ColumnDatas[6];
			const FString EventInfo = RowData.ColumnDatas[7];
			TArray<FString> ComponentInfoList;//json format
			ComponentInfoList.Add(BaseInfo);
			ComponentInfoList.Add(StyleInfo);
			ComponentInfoList.Add(QueryRule);
			ComponentInfoList.Add(StaticData);
			ComponentInfoList.Add(EventInfo);

			TSharedPtr<FJsonObject> JsonObject;
			TSharedRef< TJsonReader<TCHAR> > JsonReader = TJsonReaderFactory<TCHAR>::Create(StyleInfo);

			if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
			{
				int StyleId = JsonObject->GetIntegerField("StyleID");
				if (ComponentStyleMap.Contains(StyleId))
				{
					FString ComponentClassPath = ComponentStyleMap[StyleId].PrefabPath;
					/*UObject* tt = LoadObject<UObject>(GetWorld(), *ComponentPath);
					UMESComponetBase* t = Cast<UMESComponetBase>(tt);
					UMESComponetBase* LoadComponent = LoadObject<UMESComponetBase>(GetWorld(), *ComponentPath);*/

					UClass* ComponentClass = LoadClass<UObject>(GetWorld(), *ComponentClassPath);
					FString StyleType = JsonObject->GetStringField("StyleType");
					if (StyleType.Equals("component"))
					{
						if (UMESComponetBase::DontDestroyMap.Contains(StyleId))
						{
							UMESComponetBase* OldComponet = UMESComponetBase::DontDestroyMap[StyleId];
							OldComponet->PageChangeSave = true;
							OldComponet->CommonData.UUID = ComponentId;
							SetComponentInfo(OldComponet, ComponentInfoList);
							ComponentsToUpdate.Add(OldComponet);
							TempMESDic.Add(ComponentId, OldComponet);
							continue;
						}
						if (ComponentClass == nullptr)
						{
							ComponentClassPath = ComponentStyleMap[0].PrefabPath;
							ComponentClass = LoadClass<UObject>(GetWorld(), *ComponentClassPath);
						}
						UMESComponetBase* LoadComponent = CreateWidget<UMESComponetBase>(GetWorld(), ComponentClass);

						if (LoadComponent)
						{
							LoadComponent->CanConstruct = true;
							LoadComponent->CommonData.UUID = ComponentId;
							LoadComponent->CommonData.StyleID = StyleId;
							SetComponentInfo(LoadComponent, ComponentInfoList);
							ComponentsToUpdate.Add(LoadComponent);
							TempMESDic.Add(ComponentId, LoadComponent);
						}
					}
					else
					{
						/*FActorSpawnParameters Param;
						Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;*/
						//AMESActorBase* LoadActor3D = GetWorld()->SpawnActor<AMESActorBase>(ComponentClass, Param);
						FTransform trans;
						AMESActorBase* LoadActor3D = Cast<AMESActorBase>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this,ComponentClass, trans));
						if (LoadActor3D)
						{
							LoadActor3D->CommonData.UUID = ComponentId;
							LoadActor3D->CommonData.StyleID = StyleId;
							SetActor3DInfo(LoadActor3D, ComponentInfoList);
							Actor3DToUpdate.Add(LoadActor3D); 
							UGameplayStatics::FinishSpawningActor(LoadActor3D, trans);
							AMESPointComponetBase* point = Cast<AMESPointComponetBase>(LoadActor3D);
							if (point == nullptr)
							{
								LoadActor3D->SetFinishForModel();
							}
							else
							{
								point->SetFinishForPoint();
							}
						}
					}
				}
			}
		}
		for (auto Iter = UMESComponetBase::DontDestroyMap.CreateIterator(); Iter; ++Iter)
		{
			UMESComponetBase* a = Iter->Value;
			if (!a->PageChangeSave)
			{
				a->RemoveFromParent();//销毁这个
			}
		}
		for (auto Iter = TempMESDic.CreateIterator(); Iter; ++Iter)
		{
			UMESComponetBase* a = Iter->Value;
			if (a->CommonData.ParentUUID != 0 && TempMESDic.Contains(a->CommonData.ParentUUID))
			{
				UMESComponetBase* ParentUMES = TempMESDic[a->CommonData.ParentUUID];
				UWidget* ContainerRoot = ParentUMES->GetRootWidget();
				UPanelWidget* parent1 = Cast<UPanelWidget>(ContainerRoot);
				parent1->AddChild(a);
			}
		}
		return true;
	}
	return false;
}

void AEditorSaveGame::MarkNewComponent(UMESComponetBase * NewComponent)
{
	if (!ComponentsToAdd.Contains(NewComponent))
	{
		ComponentsToAdd.Add(NewComponent);
	}
}

void AEditorSaveGame::MarkDeleteComponent(UMESComponetBase* DeleteComponent)
{
	if (!ComponentsToDelete.Contains(DeleteComponent) && ComponentsToUpdate.Contains(DeleteComponent))
	{
		ComponentsToDelete.Add(DeleteComponent);
		ComponentsToUpdate.Remove(DeleteComponent);
	}
	if (ComponentsToAdd.Contains(DeleteComponent))
	{
		ComponentsToAdd.Remove(DeleteComponent);
	}
	//遍历递归删除自己
	for (auto Iter = TempMESDic.CreateIterator(); Iter; ++Iter)
	{
		UMESComponetBase* a = Iter->Value;
		if (a->CommonData.ParentUUID != 0 && a->CommonData.ParentUUID == DeleteComponent->CommonData.UUID)
		{
			MarkDeleteComponent(a);
		}
	}
}

void AEditorSaveGame::RefreshMarkOfComponent()
{
	ComponentsToUpdate.Append(ComponentsToAdd);
	ComponentsToAdd.Empty();
	ComponentsToDelete.Empty();

	Actor3DToUpdate.Append(Actor3DToAdd);
	Actor3DToAdd.Empty();
	Actor3DToDelete.Empty();

	UMESOperationHistory::Get()->ClearOperation();
}

void AEditorSaveGame::RemarkNewComponent(UMESComponetBase * NewComponent)
{
	UMESGameInstance* GameInstance = UMESGameInstance::Get(GetWorld());
	if (GameInstance && SqliteTool)
	{
		FString DBPath = UMESPathUtility::GetStandaloneProjectDBPath(GameInstance->ProjectInfo_Offline.ProjectName);

		// select sqlite component data
		FString UUIDStr;
		UMESBlueprintFunctionLibrary::Int64ToString(NewComponent->CommonData.UUID, UUIDStr);
		FString SelectComponentSql = "select * from " + COMPONENT_NAME + " where LIST_ID = " + UUIDStr;
		FTableData ComponentData;
		USqliteTool::LoadTableData(DBPath, SelectComponentSql, "", ComponentData);

		// We could make sure that this NewComponent is in the array of ComponentsToUpdate before delete if there has Sqlite Data which selected by LIST_ID.
		if (ComponentData.RowDatas.Num() > 0)
		{
			if (!ComponentsToUpdate.Contains(NewComponent))
			{
				ComponentsToUpdate.Add(NewComponent);
			}
		}
		// No local data in sqlite, then add it to ComponentsToAdd.
		else
		{
			if (!ComponentsToAdd.Contains(NewComponent))
			{
				ComponentsToAdd.Add(NewComponent);
			}
		}
	}
}

void AEditorSaveGame::LoadComponentsServer(int PageId)
{
	ComponentsToAdd.Empty();
	ComponentsToDelete.Empty();
	ComponentsToUpdate.Empty();
	for (auto Iter = UMESComponetBase::DontDestroyMap.CreateIterator(); Iter; ++Iter)
	{
		UMESComponetBase* a = Iter->Value;
		a->PageChangeSave = false;
	}
	Actor3DToAdd.Empty();
	Actor3DToDelete.Empty();
	Actor3DToUpdate.Empty();

	TArray<FMQComponentData> ComponentsOfThisPage;
	FindComponentsByPageId(PageId, ComponentsOfThisPage);
	TempMESDic.Empty();
	for (FMQComponentData ComponentData : ComponentsOfThisPage)
	{
		const int64 ComponentId = ComponentData.componentId;
		const FString BaseInfo = ComponentData.baseInfo;
		const FString StyleInfo = ComponentData.styleInfo;
		const FString QueryRule = ComponentData.queryRule;
		const FString StaticData = ComponentData.staticData;
		const FString EventInfo = ComponentData.eventInfo;
		TArray<FString> ComponentInfoList;//json format
		ComponentInfoList.Add(BaseInfo);
		ComponentInfoList.Add(StyleInfo);
		ComponentInfoList.Add(QueryRule);
		ComponentInfoList.Add(StaticData);
		ComponentInfoList.Add(EventInfo);

		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef< TJsonReader<TCHAR> > JsonReader = TJsonReaderFactory<TCHAR>::Create(StyleInfo);

		if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
		{
			int StyleId = JsonObject->GetIntegerField("StyleID");
			if (ComponentStyleMap.Contains(StyleId))
			{
				FString ComponentClassPath = ComponentStyleMap[StyleId].PrefabPath;
				/*UObject* tt = LoadObject<UObject>(GetWorld(), *ComponentPath);
				UMESComponetBase* t = Cast<UMESComponetBase>(tt);
				UMESComponetBase* LoadComponent = LoadObject<UMESComponetBase>(GetWorld(), *ComponentPath);*/

				UClass* ComponentClass = LoadClass<UObject>(GetWorld(), *ComponentClassPath);
				FString StyleType = JsonObject->GetStringField("StyleType");
				if (StyleType.Equals("component"))
				{
					if (UMESComponetBase::DontDestroyMap.Contains(StyleId))
					{
						UMESComponetBase* OldComponet = UMESComponetBase::DontDestroyMap[StyleId];
						OldComponet->PageChangeSave = true;
						OldComponet->CommonData.UUID = ComponentId;
						SetComponentInfo(OldComponet, ComponentInfoList);
						ComponentsToUpdate.Add(OldComponet);
						TempMESDic.Add(ComponentId, OldComponet);
						continue;
					}
					if (ComponentClass == nullptr)
					{
						ComponentClassPath = ComponentStyleMap[0].PrefabPath;
						ComponentClass = LoadClass<UObject>(GetWorld(), *ComponentClassPath);
					}
					UMESComponetBase* LoadComponent = CreateWidget<UMESComponetBase>(GetWorld(), ComponentClass);

					if (LoadComponent)
					{
						LoadComponent->CanConstruct = true;
						LoadComponent->CommonData.UUID = ComponentId;
						LoadComponent->CommonData.StyleID = StyleId;
						SetComponentInfo(LoadComponent, ComponentInfoList);
						TempMESDic.Add(ComponentId,LoadComponent);
						ComponentsToUpdate.Add(LoadComponent);
					}
				}
				else
				{
					FTransform trans;
					AMESActorBase* LoadActor3D = Cast<AMESActorBase>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, ComponentClass, trans));

					if (LoadActor3D)
					{
						LoadActor3D->CommonData.UUID = ComponentId;
						LoadActor3D->CommonData.StyleID = StyleId;
						SetActor3DInfo(LoadActor3D, ComponentInfoList);
						Actor3DToUpdate.Add(LoadActor3D);
						UGameplayStatics::FinishSpawningActor(LoadActor3D, trans);
						AMESPointComponetBase* point = Cast<AMESPointComponetBase>(LoadActor3D);
						if (point == nullptr)
						{
							LoadActor3D->SetFinishForModel();
						}
						else
						{
							point->SetFinishForPoint();
						}
					}
				}
			}
		}
	}

	for (auto Iter = UMESComponetBase::DontDestroyMap.CreateIterator(); Iter; ++Iter)
	{
		UMESComponetBase* a = Iter->Value;
		if (!a->PageChangeSave)
		{
			a->RemoveFromParent();//销毁这个
		}
	}
	for (auto Iter = TempMESDic.CreateIterator(); Iter; ++Iter)
	{
		UMESComponetBase* a = Iter->Value;
		if (a->CommonData.ParentUUID != 0 && TempMESDic.Contains(a->CommonData.ParentUUID))
		{
			UMESComponetBase* ParentUMES = TempMESDic[a->CommonData.ParentUUID];
			UWidget* ContainerRoot = ParentUMES->GetRootWidget();
			UPanelWidget* parent1 = Cast<UPanelWidget>(ContainerRoot);
			parent1->AddChild(a);
		}
	}
}

void AEditorSaveGame::FindComponentsByPageId(int PageId, TArray<FMQComponentData>& OutTargetComponents)
{
	FMQPageData TargetPageData = GetPageData(PageId);
	for (FMQPageComponents PageComponents : TargetPageData.pageComponents)
	{
		int64 TargetComponentId = PageComponents.componentId;
		for (FMQComponentData ComponentData : ComponentMap)
		{
			if (ComponentData.componentId == TargetComponentId)
			{
				OutTargetComponents.Add(ComponentData);
				break;
			}
		}
	}
}

UMESStyleBase* AEditorSaveGame::LoadStyle(FString StylePath)
{
	UClass* ComponentClass = LoadClass<UMESStyleBase>(GetWorld(), *StylePath);
	UMESStyleBase* result = CreateWidget<UMESStyleBase>(GetWorld(), ComponentClass);
	return result;
}

void AEditorSaveGame::MarkNewActor3D(AMESActorBase * NewActor3D)
{
	if (!Actor3DToAdd.Contains(NewActor3D))
	{
		Actor3DToAdd.Add(NewActor3D);
	}
}

void AEditorSaveGame::MarkDeleteActor3D(AMESActorBase * DeleteActor3D)
{
	if (!Actor3DToDelete.Contains(DeleteActor3D) && Actor3DToUpdate.Contains(DeleteActor3D))
	{
		Actor3DToDelete.Add(DeleteActor3D);
		Actor3DToUpdate.Remove(DeleteActor3D);
	}
	if (Actor3DToAdd.Contains(DeleteActor3D))
	{
		Actor3DToAdd.Remove(DeleteActor3D);
	}
}

FString AEditorSaveGame::GetStylePathFromPrefab(FString PrefabPath)
{	
	FString PPath = PrefabPath.Left(PrefabPath.Len() - 1) + TEXT("_C'");
	int LastIndex = PrefabPath.FindLastCharByPredicate([](const TCHAR& c) {return c == '.'; });
	FString FileName = PrefabPath.Right(PrefabPath.Len() - LastIndex);
	FileName = FileName.Left(FileName.Len() - 1);
	FString StylePath = PrefabPath.Left(LastIndex);
	StylePath += TEXT("_S") + FileName + "_S_C'";
	return StylePath;
}

UClass* AEditorSaveGame::LoadClassFromPath(UObject* Outer,FString blueprintPath)
{
	UClass* ComponentClass = LoadClass<UObject>(Outer, *blueprintPath);
	return ComponentClass;
}

UObject* AEditorSaveGame::LoadObjectFromPath(UObject* Outer, FString blueprintPath)
{
	
	FString symbol = TEXT("'");
	int FirstIndex = blueprintPath.Find(symbol);
	if (FirstIndex != -1)
	{
		int LastIndex = blueprintPath.FindLastCharByPredicate([](const TCHAR& c) {return c == '.'; });
		blueprintPath = blueprintPath.Left(blueprintPath.Len() - 1).Right(blueprintPath.Len() - FirstIndex - 2);
	}
	UObject* result = LoadObject<UObject>(NULL, *blueprintPath, nullptr, LOAD_Quiet | LOAD_NoWarn);
	return result;
}

void AEditorSaveGame::FindMenuAndDo(FMQMenuData & Parent, int TargetMenuId, FMQFindMenuCallback Callback, bool& IsFind)
{
	if (IsFind)
	{
		return;
	}

	if (Parent.menuId == TargetMenuId)
	{
		IsFind = true;
		Callback.Execute(Parent);
	}
	else
	{
		for (FMQMenuData& Child : Parent.ChildMenuList)
		{
			FindMenuAndDo(Child, TargetMenuId, Callback, IsFind);
		}
	}
}

void AEditorSaveGame::LoadPageData(FSqliteData & SqliteData)
{
	if (SqliteData.RowDatas.Num() > 0)
	{
		PageMap.Empty();
		for (FSqliteRowData RowData : SqliteData.RowDatas)
		{
			FMQPageData PageData;
			if (RowData.ColumnDatas.Num() >= 12)
			{
				PageData.baseInfo.pageId = FCString::Atoi(*RowData.ColumnDatas[0]);
				PageData.baseInfo.menuId = FCString::Atoi(*RowData.ColumnDatas[1]);
				PageData.baseInfo.sceneId = FCString::Atoi(*RowData.ColumnDatas[2]);
				PageData.baseInfo.uiMaskImage = FCString::Atoi(*RowData.ColumnDatas[3]);
				PageData.baseInfo.pageAnimation = *RowData.ColumnDatas[4];
				PageData.baseInfo.pageSkyBox = FCString::Atoi(*RowData.ColumnDatas[5]);
				PageData.baseInfo.environmentParam = *RowData.ColumnDatas[6];
				PageData.baseInfo.isPublish = FCString::Atoi(*RowData.ColumnDatas[7]);
				PageData.baseInfo.createUser = *RowData.ColumnDatas[8];
				PageData.baseInfo.createTime = *RowData.ColumnDatas[9];
				PageData.baseInfo.modifyUser = *RowData.ColumnDatas[10];
				PageData.baseInfo.modifyTime = *RowData.ColumnDatas[11];
				PageMap.Add(PageData);
			}
		}
	}
}

void AEditorSaveGame::LoadMenuData(FSqliteData & SqliteData)
{
	if (SqliteData.RowDatas.Num() > 0)
	{
		MenuMap.Empty();
		TArray<FMQMenuData> LoadMenuArray;
		for (FSqliteRowData RowData : SqliteData.RowDatas)
		{
			FMQMenuData MenuData;
			if (RowData.ColumnDatas.Num() >= 5)
			{
				MenuData.menuId = FCString::Atoi(*RowData.ColumnDatas[0]);
				MenuData.menuName = *RowData.ColumnDatas[1];
				MenuData.parentId = FCString::Atoi(*RowData.ColumnDatas[2]);
				MenuData.MenuLevel = FCString::Atoi(*RowData.ColumnDatas[3]);
				MenuData.isLeaf = FCString::Atoi(*RowData.ColumnDatas[4]) != 0;
				LoadMenuArray.Add(MenuData);
			}
		}

		if (LoadMenuArray.Num() > 0)
		{
			LoadMenuArray.Sort([](FMQMenuData X, FMQMenuData Y) {
				if (X.MenuLevel == Y.MenuLevel)
				{
					return X.menuId < Y.menuId;
				}
				else
				{
					return X.MenuLevel > Y.MenuLevel;
				}
			});

			for (int i = 0; i < LoadMenuArray.Num(); ++i)
			{
				FMQMenuData ChildMenu = LoadMenuArray[i];
				for (int j = i + 1; j < LoadMenuArray.Num(); ++j)
				{
					FMQMenuData& ParentMenu = LoadMenuArray[j];
					if (ChildMenu.parentId == ParentMenu.menuId)
					{
						ParentMenu.ChildMenuList.Add(ChildMenu);
						break;
					}
				}

				if (ChildMenu.MenuLevel <= 0)
				{
					MenuMap.Add(ChildMenu);
				}
			}
		}
	}
}

void AEditorSaveGame::OnTreeviewFilterChanged()
{
	FilterMenuMap.Empty();
	for (const FMQMenuData MenuData : MenuMap)
	{
		if (TreeViewFilter->PassesFilter(MenuData))
		{
			UE_LOG(LogTemp, Warning, TEXT("filter passed: %s"), *MenuData.menuName);
			FilterMenuMap.Add(MenuData);
		}
	}
	BP_OnTreeviewFilterChanged();
}

void AEditorSaveGame::MenuDataToString(const FMQMenuData& MenuData, TArray<FString>& OutSearchStrings)
{
	OutSearchStrings.Add(MenuData.menuName);
	for (FMQMenuData ChildMenu : MenuData.ChildMenuList)
	{
		MenuDataToString(ChildMenu, OutSearchStrings);
	}
}

void AEditorSaveGame::OnPageComponentFilterChanged()
{
	if (PageComponentFilterCallback.IsBound())
	{
		PageComponentFilterCallback.Execute();
		PageComponentFilterCallback.Unbind();
	}
}

void AEditorSaveGame::PageComponentToString(const UMESComponetBase * PageComponent, TArray<FString>& OutSearchStrings)
{
	FString PageComponentName = UKismetSystemLibrary::GetDisplayName(PageComponent);
	OutSearchStrings.Add(PageComponentName);
}

void AEditorSaveGame::GetComponentInfo(const UMESComponetBase * Component, TArray<FString>& OutInfoJsonStrings)
{
	OutInfoJsonStrings.Empty();

	if (Component)
	{
		FString BaseInfo = "";//MESComponentBase.CommonData
		FString StyleInfo = "";//MESComponentBase.CommonData.StyleID
		FString QueryRule = ""; //MESComponentBase.DataSourceType,DataGroup,TableName,DataBinds,FilterRules,Sorts,MaxNum
		FString StaticData = "";//MESComponentBase.ConfigJson
		FString EventInfo = "";//MESComponentBase.Events

		//base info
		TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> BaseInfoWriter = TJsonWriterFactory< TCHAR, TCondensedJsonPrintPolicy<TCHAR> >::Create(&BaseInfo);
		BaseInfoWriter->WriteObjectStart();
		BaseInfoWriter->WriteValue("ComponentName", Component->CommonData.ComponetName);
		BaseInfoWriter->WriteValue("ColorIndex", Component->CommonData.ColorIndex);
		BaseInfoWriter->WriteValue("BgAlpha", Component->CommonData.BgAlpha);
		BaseInfoWriter->WriteValue("Depth", Component->CommonData.Depth);
		FString parentStr = "";
		UMESBlueprintFunctionLibrary::Int64ToString(Component->CommonData.ParentUUID, parentStr);
		BaseInfoWriter->WriteValue("ParentUUID", parentStr);
		BaseInfoWriter->WriteValue("Visible", Component->CommonData.Visible);
		BaseInfoWriter->WriteValue("Pos", Component->CommonData.Pos.ToString());
		BaseInfoWriter->WriteValue("Size", Component->CommonData.Size.ToString());

		BaseInfoWriter->WriteObjectEnd();
		BaseInfoWriter->Close();


		//style info
		TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> StyleInfoWriter = TJsonWriterFactory< TCHAR, TCondensedJsonPrintPolicy<TCHAR> >::Create(&StyleInfo);
		StyleInfoWriter->WriteObjectStart();
		StyleInfoWriter->WriteValue("StyleID", Component->CommonData.StyleID);
		if (ComponentStyleMap.Contains(Component->CommonData.StyleID))
		{
			StyleInfoWriter->WriteValue("StyleType", ComponentStyleMap[Component->CommonData.StyleID].StyleType);
		}

		StyleInfoWriter->WriteObjectEnd();
		StyleInfoWriter->Close();

		// query rule
		TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> QueryRuleInfoWriter = TJsonWriterFactory< TCHAR, TCondensedJsonPrintPolicy<TCHAR> >::Create(&QueryRule);
		QueryRuleInfoWriter->WriteObjectStart();

		QueryRuleInfoWriter->WriteValue("DataSourceType", (int)Component->DataSourceType);
		QueryRuleInfoWriter->WriteValue("DataGroup", Component->DataGroup);
		QueryRuleInfoWriter->WriteValue("TableName", Component->TableName);

		QueryRuleInfoWriter->WriteArrayStart("DataBinds");
		for (auto DataBind : Component->DataBinds)
		{
			QueryRuleInfoWriter->WriteObjectStart();
			QueryRuleInfoWriter->WriteValue("ColumnName", DataBind.Key);
			QueryRuleInfoWriter->WriteValue("BindType", (int)DataBind.Value.BindType);
			QueryRuleInfoWriter->WriteValue("FilterValue", DataBind.Value.FilterValue);
			QueryRuleInfoWriter->WriteObjectEnd();
		}
		QueryRuleInfoWriter->WriteArrayEnd();

		QueryRuleInfoWriter->WriteArrayStart("FilterRules");
		for (FFilterRule FilterRule : Component->FiterRules)
		{
			QueryRuleInfoWriter->WriteObjectStart();
			QueryRuleInfoWriter->WriteValue("ColumnName", FilterRule.ColumnName);
			QueryRuleInfoWriter->WriteValue("FilterType", (int)FilterRule.FilterType);
			QueryRuleInfoWriter->WriteValue("FilterValue", FilterRule.FilterValue);
			QueryRuleInfoWriter->WriteObjectEnd();
		}
		QueryRuleInfoWriter->WriteArrayEnd();

		QueryRuleInfoWriter->WriteArrayStart("Sorts");
		for (FDataSort DataSort : Component->Sorts)
		{
			QueryRuleInfoWriter->WriteObjectStart();
			QueryRuleInfoWriter->WriteValue("ColumnName", DataSort.ColumnName);
			QueryRuleInfoWriter->WriteValue("SortType", (int)DataSort.SortType);
			QueryRuleInfoWriter->WriteObjectEnd();
		}
		QueryRuleInfoWriter->WriteArrayEnd();

		QueryRuleInfoWriter->WriteValue("MaxNum", Component->MaxNum);

		QueryRuleInfoWriter->WriteObjectEnd();
		QueryRuleInfoWriter->Close();


		// static data
		TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> StaticDataWriter = TJsonWriterFactory< TCHAR, TCondensedJsonPrintPolicy<TCHAR> >::Create(&StaticData);
		StaticDataWriter->WriteObjectStart();
		StaticDataWriter->WriteArrayStart("CustomConfig");

		for (auto CustomConfig : Component->CustomConfig)
		{
			StaticDataWriter->WriteObjectStart();
			StaticDataWriter->WriteValue("ConfigName", CustomConfig.Key);
			StaticDataWriter->WriteValue("Category", CustomConfig.Value.Category);
			StaticDataWriter->WriteValue("FiledType", CustomConfig.Value.FiledType);
			StaticDataWriter->WriteValue("FiledValue", CustomConfig.Value.FiledValue);
			StaticDataWriter->WriteObjectEnd();
		}

		StaticDataWriter->WriteArrayEnd();
		StaticDataWriter->WriteObjectEnd();
		StaticDataWriter->Close();


		// event info
		TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> EventInfoWriter = TJsonWriterFactory< TCHAR, TCondensedJsonPrintPolicy<TCHAR> >::Create(&EventInfo);
		EventInfoWriter->WriteObjectStart();
		EventInfoWriter->WriteArrayStart("Events");
		for (FMESEvents Event : Component->Events)
		{
			EventInfoWriter->WriteObjectStart();
			EventInfoWriter->WriteValue("TriggerMethod", Event.TriggerMethod);
			EventInfoWriter->WriteArrayStart("Events");
			for (FComponetEvent ComponentEvent : Event.Events)
			{
				EventInfoWriter->WriteObjectStart();
				EventInfoWriter->WriteValue("EventType", (int)ComponentEvent.EventType);
				EventInfoWriter->WriteValue("EventName", ComponentEvent.EventName);
				EventInfoWriter->WriteValue("EventParam", ComponentEvent.EventParam);
				EventInfoWriter->WriteObjectEnd();
			}
			EventInfoWriter->WriteArrayEnd();
			EventInfoWriter->WriteObjectEnd();
		}
		EventInfoWriter->WriteArrayEnd();

		EventInfoWriter->WriteObjectEnd();
		EventInfoWriter->Close();

		// add result
		OutInfoJsonStrings.Add(BaseInfo);
		OutInfoJsonStrings.Add(StyleInfo);
		OutInfoJsonStrings.Add(QueryRule);
		OutInfoJsonStrings.Add(StaticData);
		OutInfoJsonStrings.Add(EventInfo);
	}
}

void AEditorSaveGame::SetComponentInfo(UMESComponetBase * Component, const TArray<FString>& InfoJsonStrings)
{
	if (Component && InfoJsonStrings.Num() >= 5)
	{
		FString BaseInfo = InfoJsonStrings[0];
		FString QueryRule = InfoJsonStrings[2];
		FString StaticData = InfoJsonStrings[3];
		FString EventInfo = InfoJsonStrings[4];

		// base info
		TSharedPtr<FJsonObject> BaseInfoJsonObject;
		TSharedRef< TJsonReader<TCHAR> > BaseInfoJsonReader = TJsonReaderFactory<TCHAR>::Create(BaseInfo);
		if (FJsonSerializer::Deserialize(BaseInfoJsonReader, BaseInfoJsonObject, FJsonSerializer::EFlags::StoreNumbersAsStrings))
		{
			Component->CommonData.ComponetName = BaseInfoJsonObject->GetStringField("ComponentName");
			Component->CommonData.ColorIndex = BaseInfoJsonObject->GetNumberField("ColorIndex");
			Component->CommonData.BgAlpha = BaseInfoJsonObject->GetNumberField("BgAlpha");
			Component->CommonData.Depth = BaseInfoJsonObject->GetNumberField("Depth");
			FString ParrentUUID = BaseInfoJsonObject->GetStringField(("ParentUUID"));
			Component->CommonData.ParentUUID = FCString::Atoi64((*ParrentUUID));
			Component->CommonData.Visible = BaseInfoJsonObject->GetBoolField("Visible");
			Component->CommonData.Pos.InitFromString(BaseInfoJsonObject->GetStringField("Pos"));
			Component->CommonData.Size.InitFromString(BaseInfoJsonObject->GetStringField("Size"));
		}

		// query rule
		TSharedPtr<FJsonObject> QueryRuleJsonObject;
		TSharedRef< TJsonReader<TCHAR> > QueryRuleJsonReader = TJsonReaderFactory<TCHAR>::Create(QueryRule);
		if (FJsonSerializer::Deserialize(QueryRuleJsonReader, QueryRuleJsonObject))
		{
			Component->DataSourceType = (EDataSourceType)QueryRuleJsonObject->GetIntegerField("DataSourceType");
			Component->DataGroup = QueryRuleJsonObject->GetStringField("DataGroup");
			Component->TableName = QueryRuleJsonObject->GetStringField("TableName");
			TArray<TSharedPtr<FJsonValue>> JsonDataBindArray = QueryRuleJsonObject->GetArrayField("DataBinds");
			Component->DataBinds.Reset();
			for (TSharedPtr<FJsonValue> JsonDataBind : JsonDataBindArray)
			{
				TSharedPtr<FJsonObject> ChildJsonObject = JsonDataBind->AsObject();
				FDataBind DataBind;
				FString ColumnName = ChildJsonObject->GetStringField("ColumnName");
				DataBind.BindType = (EDataBindType)ChildJsonObject->GetIntegerField("BindType");
				DataBind.FilterValue = ChildJsonObject->GetStringField("FilterValue");
				Component->DataBinds.Add(ColumnName, DataBind);
			}
			TArray<TSharedPtr<FJsonValue>> JsonFilterRuleArray = QueryRuleJsonObject->GetArrayField("FilterRules");
			Component->FiterRules.Reset();
			for (TSharedPtr<FJsonValue> JsonFilterRule : JsonFilterRuleArray)
			{
				TSharedPtr<FJsonObject> ChildJsonObject = JsonFilterRule->AsObject();
				FFilterRule FilterRule;
				FilterRule.ColumnName = ChildJsonObject->GetStringField("ColumnName");
				FilterRule.FilterType = (EFilterType)ChildJsonObject->GetIntegerField("FilterType");
				FilterRule.FilterValue = ChildJsonObject->GetStringField("FilterValue");
				Component->FiterRules.Add(FilterRule);
			}
			TArray<TSharedPtr<FJsonValue>> JsonDataSortArray = QueryRuleJsonObject->GetArrayField("Sorts");
			Component->Sorts.Reset();
			for (TSharedPtr<FJsonValue> JsonDataSort : JsonDataSortArray)
			{
				TSharedPtr<FJsonObject> ChildJsonObject = JsonDataSort->AsObject();
				FDataSort DataSort;
				DataSort.ColumnName = ChildJsonObject->GetStringField("ColumnName");
				DataSort.SortType = (EDataSortType)ChildJsonObject->GetIntegerField("SortType");
				Component->Sorts.Add(DataSort);
			}
			Component->MaxNum = QueryRuleJsonObject->GetIntegerField("MaxNum");
		}

		//static data
		TSharedPtr<FJsonObject> StaticDataJsonObject;
		TSharedRef< TJsonReader<TCHAR> > StaticDataJsonReader = TJsonReaderFactory<TCHAR>::Create(StaticData);
		if (FJsonSerializer::Deserialize(StaticDataJsonReader, StaticDataJsonObject))
		{
			TArray<TSharedPtr<FJsonValue>> JsonCustomConfigArray = StaticDataJsonObject->GetArrayField("CustomConfig");
			for (TSharedPtr<FJsonValue> JsonCustomConfig : JsonCustomConfigArray)
			{
				TSharedPtr<FJsonObject> ChildJsonObject = JsonCustomConfig->AsObject();
				FMESCustomConfig CustomConfig;
				FString ConfigName = ChildJsonObject->GetStringField("ConfigName");
				CustomConfig.Category = ChildJsonObject->GetStringField("Category");
				CustomConfig.FiledType = ChildJsonObject->GetStringField("FiledType");
				CustomConfig.FiledValue = ChildJsonObject->GetStringField("FiledValue");
				if (Component->CustomConfig.Contains(ConfigName))
				{
					Component->CustomConfig[ConfigName] = CustomConfig;
				}
			}
		}

		//event info
		TSharedPtr<FJsonObject> EventInfoJsonObject;
		TSharedRef< TJsonReader<TCHAR> > EventInfoJsonReader = TJsonReaderFactory<TCHAR>::Create(EventInfo);
		if (FJsonSerializer::Deserialize(EventInfoJsonReader, EventInfoJsonObject))
		{
			TArray<TSharedPtr<FJsonValue>> JsonEventsArray = EventInfoJsonObject->GetArrayField("Events");
			Component->Events.Reset();
			for (TSharedPtr<FJsonValue> JsonEvent : JsonEventsArray)
			{
				TSharedPtr<FJsonObject> ChildJsonObject = JsonEvent->AsObject();
				FMESEvents Event;
				Event.TriggerMethod = ChildJsonObject->GetStringField("TriggerMethod");

				TArray<TSharedPtr<FJsonValue>> JsonEventInfoArray = ChildJsonObject->GetArrayField("Events");
				for (TSharedPtr<FJsonValue> JsonEventInfo : JsonEventInfoArray)
				{
					TSharedPtr<FJsonObject> EventJsonObject = JsonEventInfo->AsObject();
					FComponetEvent ComponentEvent;
					ComponentEvent.EventType = (EMESComponetEventType)EventJsonObject->GetIntegerField("EventType");
					ComponentEvent.EventName = EventJsonObject->GetStringField("EventName");
					ComponentEvent.EventParam = EventJsonObject->GetStringField("EventParam");
					Event.Events.Add(ComponentEvent);
				}

				Component->Events.Add(Event);
			}
		}
	}
}

void AEditorSaveGame::GetActor3DInfo(const AMESActorBase * Actor3D, TArray<FString>& OutInfoJsonStrings)
{
	OutInfoJsonStrings.Empty();

	if (Actor3D)
	{
		FString BaseInfo = "";//AMESActorBase.CommonData
		FString StyleInfo = "";//AMESActorBase.CommonData.StyleID and StyleType
		FString QueryRule = ""; //AMESActorBase.DataSourceType,DataGroup,TableName,DataBinds,FilterRules,Sorts,MaxNum
		FString StaticData = "";//AMESActorBase.CustomConfig
		FString EventInfo = "";//AMESActorBase.Events

		//base info
		TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> BaseInfoWriter = TJsonWriterFactory< TCHAR, TCondensedJsonPrintPolicy<TCHAR> >::Create(&BaseInfo);
		BaseInfoWriter->WriteObjectStart();
		BaseInfoWriter->WriteValue("ComponentName", Actor3D->CommonData.ComponetName);
		BaseInfoWriter->WriteValue("ColorIndex", Actor3D->CommonData.ColorIndex);
		BaseInfoWriter->WriteValue("BgAlpha", Actor3D->CommonData.BgAlpha);
		BaseInfoWriter->WriteValue("Visible", Actor3D->CommonData.Visible);
		BaseInfoWriter->WriteValue("Pos", Actor3D->CommonData.Pos.ToString());
		BaseInfoWriter->WriteValue("Rotation", Actor3D->CommonData.Rotation.ToString());
		BaseInfoWriter->WriteValue("Size", Actor3D->CommonData.Size.ToString());

		BaseInfoWriter->WriteObjectEnd();
		BaseInfoWriter->Close();


		//style info
		TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> StyleInfoWriter = TJsonWriterFactory< TCHAR, TCondensedJsonPrintPolicy<TCHAR> >::Create(&StyleInfo);
		StyleInfoWriter->WriteObjectStart();
		StyleInfoWriter->WriteValue("StyleID", Actor3D->CommonData.StyleID);
		if (ComponentStyleMap.Contains(Actor3D->CommonData.StyleID))
		{
			StyleInfoWriter->WriteValue("StyleType", ComponentStyleMap[Actor3D->CommonData.StyleID].StyleType);
		}

		StyleInfoWriter->WriteObjectEnd();
		StyleInfoWriter->Close();

		// query rule
		TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> QueryRuleInfoWriter = TJsonWriterFactory< TCHAR, TCondensedJsonPrintPolicy<TCHAR> >::Create(&QueryRule);
		QueryRuleInfoWriter->WriteObjectStart();

		QueryRuleInfoWriter->WriteValue("DataSourceType", (int)Actor3D->DataSourceType);
		QueryRuleInfoWriter->WriteValue("DataGroup", Actor3D->DataGroup);
		QueryRuleInfoWriter->WriteValue("TableName", Actor3D->TableName);

		QueryRuleInfoWriter->WriteArrayStart("DataBinds");
		for (auto DataBind : Actor3D->DataBinds)
		{
			QueryRuleInfoWriter->WriteObjectStart();
			QueryRuleInfoWriter->WriteValue("ColumnName", DataBind.Key);
			QueryRuleInfoWriter->WriteValue("BindType", (int)DataBind.Value.BindType);
			QueryRuleInfoWriter->WriteValue("FilterValue", DataBind.Value.FilterValue);
			QueryRuleInfoWriter->WriteObjectEnd();
		}
		QueryRuleInfoWriter->WriteArrayEnd();

		QueryRuleInfoWriter->WriteArrayStart("FilterRules");
		for (FFilterRule FilterRule : Actor3D->FiterRules)
		{
			QueryRuleInfoWriter->WriteObjectStart();
			QueryRuleInfoWriter->WriteValue("ColumnName", FilterRule.ColumnName);
			QueryRuleInfoWriter->WriteValue("FilterType", (int)FilterRule.FilterType);
			QueryRuleInfoWriter->WriteValue("FilterValue", FilterRule.FilterValue);
			QueryRuleInfoWriter->WriteObjectEnd();
		}
		QueryRuleInfoWriter->WriteArrayEnd();

		QueryRuleInfoWriter->WriteArrayStart("Sorts");
		for (FDataSort DataSort : Actor3D->Sorts)
		{
			QueryRuleInfoWriter->WriteObjectStart();
			QueryRuleInfoWriter->WriteValue("ColumnName", DataSort.ColumnName);
			QueryRuleInfoWriter->WriteValue("SortType", (int)DataSort.SortType);
			QueryRuleInfoWriter->WriteObjectEnd();
		}
		QueryRuleInfoWriter->WriteArrayEnd();

		QueryRuleInfoWriter->WriteValue("MaxNum", Actor3D->MaxNum);

		QueryRuleInfoWriter->WriteObjectEnd();
		QueryRuleInfoWriter->Close();


		// static data
		TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> StaticDataWriter = TJsonWriterFactory< TCHAR, TCondensedJsonPrintPolicy<TCHAR> >::Create(&StaticData);
		StaticDataWriter->WriteObjectStart();
		StaticDataWriter->WriteArrayStart("CustomConfig");

		for (auto CustomConfig : Actor3D->CustomConfig)
		{
			StaticDataWriter->WriteObjectStart();
			StaticDataWriter->WriteValue("ConfigName", CustomConfig.Key);
			StaticDataWriter->WriteValue("Category", CustomConfig.Value.Category);
			StaticDataWriter->WriteValue("FiledType", CustomConfig.Value.FiledType);
			StaticDataWriter->WriteValue("FiledValue", CustomConfig.Value.FiledValue);
			StaticDataWriter->WriteObjectEnd();
		}

		StaticDataWriter->WriteArrayEnd();
		StaticDataWriter->WriteObjectEnd();
		StaticDataWriter->Close();


		// event info
		TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> EventInfoWriter = TJsonWriterFactory< TCHAR, TCondensedJsonPrintPolicy<TCHAR> >::Create(&EventInfo);
		EventInfoWriter->WriteObjectStart();
		EventInfoWriter->WriteArrayStart("Events");
		for (FMESEvents Event : Actor3D->Events)
		{
			EventInfoWriter->WriteObjectStart();
			EventInfoWriter->WriteValue("TriggerMethod", Event.TriggerMethod);
			EventInfoWriter->WriteArrayStart("Events");
			for (FComponetEvent ComponentEvent : Event.Events)
			{
				EventInfoWriter->WriteObjectStart();
				EventInfoWriter->WriteValue("EventType", (int)ComponentEvent.EventType);
				EventInfoWriter->WriteValue("EventName", ComponentEvent.EventName);
				EventInfoWriter->WriteValue("EventParam", ComponentEvent.EventParam);
				EventInfoWriter->WriteObjectEnd();
			}
			EventInfoWriter->WriteArrayEnd();
			EventInfoWriter->WriteObjectEnd();
		}
		EventInfoWriter->WriteArrayEnd();

		EventInfoWriter->WriteObjectEnd();
		EventInfoWriter->Close();

		// add result
		OutInfoJsonStrings.Add(BaseInfo);
		OutInfoJsonStrings.Add(StyleInfo);
		OutInfoJsonStrings.Add(QueryRule);
		OutInfoJsonStrings.Add(StaticData);
		OutInfoJsonStrings.Add(EventInfo);
	}
}

void AEditorSaveGame::SetActor3DInfo(AMESActorBase * Actor3D, const TArray<FString>& InfoJsonStrings)
{
	if (Actor3D && InfoJsonStrings.Num() >= 5)
	{
		FString BaseInfo = InfoJsonStrings[0];
		FString QueryRule = InfoJsonStrings[2];
		FString StaticData = InfoJsonStrings[3];
		FString EventInfo = InfoJsonStrings[4];

		// base info
		TSharedPtr<FJsonObject> BaseInfoJsonObject;
		TSharedRef< TJsonReader<TCHAR> > BaseInfoJsonReader = TJsonReaderFactory<TCHAR>::Create(BaseInfo);
		if (FJsonSerializer::Deserialize(BaseInfoJsonReader, BaseInfoJsonObject))
		{
			Actor3D->CommonData.ComponetName = BaseInfoJsonObject->GetStringField("ComponentName");
			Actor3D->CommonData.ColorIndex = BaseInfoJsonObject->GetNumberField("ColorIndex");
			Actor3D->CommonData.BgAlpha = BaseInfoJsonObject->GetNumberField("BgAlpha");
			Actor3D->CommonData.Visible = BaseInfoJsonObject->GetBoolField("Visible");
			Actor3D->CommonData.Pos.InitFromString(BaseInfoJsonObject->GetStringField("Pos"));
			Actor3D->CommonData.Rotation.InitFromString(BaseInfoJsonObject->GetStringField("Rotation"));
			Actor3D->CommonData.Size.InitFromString(BaseInfoJsonObject->GetStringField("Size"));
		}

		// query rule
		TSharedPtr<FJsonObject> QueryRuleJsonObject;
		TSharedRef< TJsonReader<TCHAR> > QueryRuleJsonReader = TJsonReaderFactory<TCHAR>::Create(QueryRule);
		if (FJsonSerializer::Deserialize(QueryRuleJsonReader, QueryRuleJsonObject))
		{
			Actor3D->DataSourceType = (EDataSourceType)QueryRuleJsonObject->GetIntegerField("DataSourceType");
			Actor3D->DataGroup = QueryRuleJsonObject->GetStringField("DataGroup");
			Actor3D->TableName = QueryRuleJsonObject->GetStringField("TableName");
			TArray<TSharedPtr<FJsonValue>> JsonDataBindArray = QueryRuleJsonObject->GetArrayField("DataBinds");
			Actor3D->DataBinds.Reset();
			for (TSharedPtr<FJsonValue> JsonDataBind : JsonDataBindArray)
			{
				TSharedPtr<FJsonObject> ChildJsonObject = JsonDataBind->AsObject();
				FDataBind DataBind;
				FString ColumnName = ChildJsonObject->GetStringField("ColumnName");
				DataBind.BindType = (EDataBindType)ChildJsonObject->GetIntegerField("BindType");
				DataBind.FilterValue = ChildJsonObject->GetStringField("FilterValue");
				Actor3D->DataBinds.Add(ColumnName, DataBind);
			}
			TArray<TSharedPtr<FJsonValue>> JsonFilterRuleArray = QueryRuleJsonObject->GetArrayField("FilterRules");
			Actor3D->FiterRules.Reset();
			for (TSharedPtr<FJsonValue> JsonFilterRule : JsonFilterRuleArray)
			{
				TSharedPtr<FJsonObject> ChildJsonObject = JsonFilterRule->AsObject();
				FFilterRule FilterRule;
				FilterRule.ColumnName = ChildJsonObject->GetStringField("ColumnName");
				FilterRule.FilterType = (EFilterType)ChildJsonObject->GetIntegerField("FilterType");
				FilterRule.FilterValue = ChildJsonObject->GetStringField("FilterValue");
				Actor3D->FiterRules.Add(FilterRule);
			}
			TArray<TSharedPtr<FJsonValue>> JsonDataSortArray = QueryRuleJsonObject->GetArrayField("Sorts");
			Actor3D->Sorts.Reset();
			for (TSharedPtr<FJsonValue> JsonDataSort : JsonDataSortArray)
			{
				TSharedPtr<FJsonObject> ChildJsonObject = JsonDataSort->AsObject();
				FDataSort DataSort;
				DataSort.ColumnName = ChildJsonObject->GetStringField("ColumnName");
				DataSort.SortType = (EDataSortType)ChildJsonObject->GetIntegerField("SortType");
				Actor3D->Sorts.Add(DataSort);
			}
			Actor3D->MaxNum = QueryRuleJsonObject->GetIntegerField("MaxNum");
		}

		//static data
		TSharedPtr<FJsonObject> StaticDataJsonObject;
		TSharedRef< TJsonReader<TCHAR> > StaticDataJsonReader = TJsonReaderFactory<TCHAR>::Create(StaticData);
		if (FJsonSerializer::Deserialize(StaticDataJsonReader, StaticDataJsonObject))
		{
			TArray<TSharedPtr<FJsonValue>> JsonCustomConfigArray = StaticDataJsonObject->GetArrayField("CustomConfig");
			for (TSharedPtr<FJsonValue> JsonCustomConfig : JsonCustomConfigArray)
			{
				TSharedPtr<FJsonObject> ChildJsonObject = JsonCustomConfig->AsObject();
				FMESCustomConfig CustomConfig;
				FString ConfigName = ChildJsonObject->GetStringField("ConfigName");
				CustomConfig.Category = ChildJsonObject->GetStringField("Category");
				CustomConfig.FiledType = ChildJsonObject->GetStringField("FiledType");
				CustomConfig.FiledValue = ChildJsonObject->GetStringField("FiledValue");
				if (Actor3D->CustomConfig.Contains(ConfigName))
				{
					Actor3D->CustomConfig[ConfigName] = CustomConfig;
				}
			}
		}

		//event info
		TSharedPtr<FJsonObject> EventInfoJsonObject;
		TSharedRef< TJsonReader<TCHAR> > EventInfoJsonReader = TJsonReaderFactory<TCHAR>::Create(EventInfo);
		if (FJsonSerializer::Deserialize(EventInfoJsonReader, EventInfoJsonObject))
		{
			TArray<TSharedPtr<FJsonValue>> JsonEventsArray = EventInfoJsonObject->GetArrayField("Events");
			Actor3D->Events.Reset();
			for (TSharedPtr<FJsonValue> JsonEvent : JsonEventsArray)
			{
				TSharedPtr<FJsonObject> ChildJsonObject = JsonEvent->AsObject();
				FMESEvents Event;
				Event.TriggerMethod = ChildJsonObject->GetStringField("TriggerMethod");

				TArray<TSharedPtr<FJsonValue>> JsonEventInfoArray = ChildJsonObject->GetArrayField("Events");
				for (TSharedPtr<FJsonValue> JsonEventInfo : JsonEventInfoArray)
				{
					TSharedPtr<FJsonObject> EventJsonObject = JsonEventInfo->AsObject();
					FComponetEvent ComponentEvent;
					ComponentEvent.EventType = (EMESComponetEventType)EventJsonObject->GetIntegerField("EventType");
					ComponentEvent.EventName = EventJsonObject->GetStringField("EventName");
					ComponentEvent.EventParam = EventJsonObject->GetStringField("EventParam");
					Event.Events.Add(ComponentEvent);
				}

				Actor3D->Events.Add(Event);
			}
		}
	}
}
