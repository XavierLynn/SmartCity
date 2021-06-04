// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "JsonUtilities/Public/JsonUtilities.h"
#include "MESProduct/MESComponent/ComponetBase/MESComponetBase.h"
#include "RabbitMqProtocol.generated.h"


/**
 * 
 */

//copy from ME
#define PROTOCOL_RESP_CMDID_PREF 0x10000000


#define RABBITMQ_HEARTBEAT_REQUEST 10010001
#define RABBITMQ_HEARTBEAT_RESPONSE RABBITMQ_HEARTBEAT_REQUEST | PROTOCOL_RESP_CMDID_PREF



#define RABBITMQ_LOGIN_REQUEST 10010002
#define RABBITMQ_LOGIN_RESPONSE RABBITMQ_LOGIN_REQUEST | PROTOCOL_RESP_CMDID_PREF
USTRUCT(BlueprintType)
struct FMQLoginRequest
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString UserName;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString Password;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int ClientType;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool AutoLogin;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int RunnerType;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString TicketId;


	FString ToString(FString Credentials)
	{
		FString Result("{\"consumer\":\"");
		Result.Append(UserName);
		Result.Append("\",\"credentials\":\"");
		Result.Append(Credentials);
		Result.Append("\",\"clientType\":");
		Result.Append(FString::FromInt(ClientType));
		Result.Append(",\"auto\":");
		Result.Append(AutoLogin ? "true" : "false");
		Result.Append(",\"runnerType\":");
		Result.Append(FString::FromInt(RunnerType));
		Result.Append(",\"ticketId\":");
		Result.Append(TicketId.IsEmpty() ? "null" : "\"" + TicketId + "\"");
		Result.Append("}");
		return Result;
	}
};
USTRUCT(BlueprintType)
struct FMQLoginData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		int status;
	UPROPERTY(BlueprintReadWrite)
		FString message;
};
USTRUCT(BlueprintType)
struct FMQLoginResponse
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		int status;
	UPROPERTY(BlueprintReadWrite)
		FString message;
	UPROPERTY(BlueprintReadWrite)
		int type;
	UPROPERTY(BlueprintReadWrite)
		int64 fromUuid;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FMQLoginData result;
};



#define RABBITMQ_LOGOUT_REQUEST 10010003
#define RABBITMQ_LOGOUT_RESPONSE RABBITMQ_LOGOUT_REQUEST | PROTOCOL_RESP_CMDID_PREF
USTRUCT(BlueprintType)
struct FMQLogoutRequest
{
	GENERATED_BODY()


	FString ToString()
	{
		FString Result;
		Result.Append("{}");
		return Result;
	}
};




#define RABBITMQ_DATA_SOURCE_LIST_REQUEST 11010001
#define RABBITMQ_DATA_SOURCE_LIST_RESPONSE RABBITMQ_DATA_SOURCE_LIST_REQUEST | PROTOCOL_RESP_CMDID_PREF
USTRUCT(BlueprintType)
struct FMQDataSourceListRequest
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
struct FMQDataSourceGroup
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		FString dataGroup;
	UPROPERTY(BlueprintReadWrite)
		TArray<FString> dataSourceList;
};
USTRUCT(BlueprintType)
struct FMQDataSourceListResponse
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		int status;
	UPROPERTY(BlueprintReadWrite)
		FString message;
	UPROPERTY(BlueprintReadWrite)
		int type;
	UPROPERTY(BlueprintReadWrite)
		int64 fromUuid;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FMQDataSourceGroup> result;
};





#define RABBITMQ_DATA_SOURCE_COLUMN_INFO_REQUEST 11010002
#define RABBITMQ_DATA_SOURCE_COLUMN_INFO_RESPONSE RABBITMQ_DATA_SOURCE_COLUMN_INFO_REQUEST | PROTOCOL_RESP_CMDID_PREF
USTRUCT(BlueprintType)
struct FMQDataSourceColumnInfoRequest
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString dataGroup;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString dataSource;

	FString ToString()
	{
		FString Result;
		FJsonObjectConverter::UStructToJsonObjectString(*this, Result);
		return Result;
	}
};
USTRUCT(BlueprintType)
struct FMQDataSourceColumnInfoItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		FString fieldName;
	UPROPERTY(BlueprintReadWrite)
		FString type;
	UPROPERTY(BlueprintReadWrite)
		FString dataType;
};
USTRUCT(BlueprintType)
struct FMQDataSourceColumnInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		FString dataGroup;
	UPROPERTY(BlueprintReadWrite)
		FString dataSource;
	UPROPERTY(BlueprintReadWrite)
		TArray<FMQDataSourceColumnInfoItem> columns;
};
USTRUCT(BlueprintType)
struct FMQDataSourceColumnInfoResponse
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		int status;
	UPROPERTY(BlueprintReadWrite)
		FString message;
	UPROPERTY(BlueprintReadWrite)
		int type;
	UPROPERTY(BlueprintReadWrite)
		int64 fromUuid;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FMQDataSourceColumnInfo result;
};



#define RABBITMQ_DATA_SOURCE_COLUMU_DATA_REQUEST 11010003
#define RABBITMQ_DATA_SOURCE_COLUMU_DATA_RESPONSE RABBITMQ_DATA_SOURCE_COLUMU_DATA_REQUEST | PROTOCOL_RESP_CMDID_PREF
USTRUCT(BlueprintType)
struct FMQDataSourceColumnDataRequest
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString dataGroup;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString dataSource;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString fieldName;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool descending;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int dataNum;


	FString ToString()
	{
		FString Result;
		FJsonObjectConverter::UStructToJsonObjectString(*this, Result);
		return Result;
	}
};
USTRUCT(BlueprintType)
struct FMQDataSourceColumnData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		FString dataGroup;
	UPROPERTY(BlueprintReadWrite)
		FString dataSource;
	UPROPERTY(BlueprintReadWrite)
		FString fieldName;
	UPROPERTY(BlueprintReadWrite)
		int dataType;
	UPROPERTY(BlueprintReadWrite)
		TArray<FString> columnData;
	UPROPERTY(BlueprintReadWrite)
		FString min;
	UPROPERTY(BlueprintReadWrite)
		FString max;
};
USTRUCT(BlueprintType)
struct FMQDataSourceColumnDataResponse
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		int status;
	UPROPERTY(BlueprintReadWrite)
		FString message;
	UPROPERTY(BlueprintReadWrite)
		int type;
	UPROPERTY(BlueprintReadWrite)
		int64 fromUuid;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FMQDataSourceColumnData result;
};




#define RABBITMQ_PAGE_LIST_REQUEST 10030001
#define RABBITMQ_PAGE_LIST_RESPONSE RABBITMQ_PAGE_LIST_REQUEST | PROTOCOL_RESP_CMDID_PREF
USTRUCT(BlueprintType)
struct FMQPageListRequest
{
	GENERATED_BODY()

	FString ToString()
	{
		FString Result;
		Result.Append("{}");
		return Result;
	}
};
USTRUCT(BlueprintType)
struct FMQPageBaseInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int pageId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int menuId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int sceneId = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int uiMaskImage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString pageAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int pageSkyBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString environmentParam;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int isPublish;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int isLock;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString lockUser;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString createUser;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString modifyUser;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString createTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString modifyTime;
};
USTRUCT(BlueprintType)
struct FMQPageComponents
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int pageId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int64 componentId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int displayLevel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int componentFlag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int isHideInAem;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int isHideInSre;
};
USTRUCT(BlueprintType)
struct FMQPageTemplates
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int pageId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int templateId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int isHideInAem;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int isHideInSre;
};
USTRUCT(BlueprintType)
struct FMQPageData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FMQPageBaseInfo baseInfo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FMQPageComponents> pageComponents;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FMQPageTemplates> pageTemplates;
};
USTRUCT(BlueprintType)
struct FMQPageList
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FMQPageData> pages;
};
USTRUCT(BlueprintType)
struct FMQPageListResponse
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		int status;
	UPROPERTY(BlueprintReadWrite)
		FString message;
	UPROPERTY(BlueprintReadWrite)
		int type;
	UPROPERTY(BlueprintReadWrite)
		int64 fromUuid;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FMQPageList result;

	void Init(const FString& JsonString)
	{
		if (JsonString.IsEmpty())
		{
			return;
		}

		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef< TJsonReader<TCHAR> > JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonString);

		if (FJsonSerializer::Deserialize(JsonReader, JsonObject, FJsonSerializer::EFlags::StoreNumbersAsStrings))
		{
			status = JsonObject->GetIntegerField("status");
			message = JsonObject->GetStringField("message");
			type = JsonObject->GetIntegerField("type");
			fromUuid = JsonObject->GetIntegerField("fromUuid");

			TSharedPtr<FJsonObject> ResultJsonObject = JsonObject->GetObjectField("result");
			if (!ResultJsonObject.IsValid())
			{
				return;
			}

			TArray<TSharedPtr<FJsonValue>> PagesArray = ResultJsonObject->GetArrayField("pages");
			for (TSharedPtr<FJsonValue> PageValue : PagesArray)
			{
				TSharedPtr<FJsonObject> PageObject = PageValue->AsObject();
				if (PageObject)
				{
					FMQPageData PageData;

					//BaseInfo
					TSharedPtr<FJsonObject> BaseInfoObject = PageObject->GetObjectField("baseInfo");
					if (BaseInfoObject)
					{
						PageData.baseInfo.pageId = BaseInfoObject->GetIntegerField("pageId");
						PageData.baseInfo.menuId = BaseInfoObject->GetIntegerField("menuId");
						PageData.baseInfo.sceneId = BaseInfoObject->GetIntegerField("sceneId");
						PageData.baseInfo.pageAnimation = BaseInfoObject->GetStringField("pageAnimation");
						//PageData.baseInfo.pageSkyBox = BaseInfoObject->GetIntegerField("pageSkyBox");
						PageData.baseInfo.environmentParam = BaseInfoObject->GetStringField("environmentParam");
						PageData.baseInfo.isPublish = BaseInfoObject->GetIntegerField("isPublish");
						PageData.baseInfo.isLock = BaseInfoObject->GetIntegerField("isLock");
						//PageData.baseInfo.lockUser = BaseInfoObject->GetStringField("lockUser");
						PageData.baseInfo.createUser = BaseInfoObject->GetStringField("createUser");
						PageData.baseInfo.modifyUser = BaseInfoObject->GetStringField("modifyUser");
						PageData.baseInfo.createTime = BaseInfoObject->GetStringField("createTime");
						PageData.baseInfo.modifyTime = BaseInfoObject->GetStringField("modifyTime");
					}

					// Page Components
					TArray<TSharedPtr<FJsonValue>> PageComponentsArray = PageObject->GetArrayField("pageComponents");
					for (TSharedPtr<FJsonValue> PageComponentValue : PageComponentsArray)
					{
						TSharedPtr<FJsonObject> PageComponentObject = PageComponentValue->AsObject();
						if (PageComponentObject)
						{
							FMQPageComponents PageComponentData;
							PageComponentData.pageId = PageComponentObject->GetIntegerField("pageId");

							FString ComponentId = PageComponentObject->GetStringField("componentId");
							PageComponentData.componentId = FCString::Atoi64(*ComponentId);

							PageComponentData.displayLevel = PageComponentObject->GetIntegerField("displayLevel");
							PageComponentData.componentFlag = PageComponentObject->GetIntegerField("componentFlag");
							PageComponentData.isHideInAem = PageComponentObject->GetIntegerField("isHideInAem");
							PageComponentData.isHideInSre = PageComponentObject->GetIntegerField("isHideInSre");

							PageData.pageComponents.Add(PageComponentData);
						}
					}
					
					// Page Templates
					TArray<TSharedPtr<FJsonValue>> PageTemplatesArray = PageObject->GetArrayField("pageTemplates");
					for (TSharedPtr<FJsonValue> PageTemplateValue : PageTemplatesArray)
					{
						TSharedPtr<FJsonObject> PageTemplateObject = PageTemplateValue->AsObject();
						if (PageTemplateObject)
						{
							FMQPageTemplates PageTemplateData;
							PageTemplateData.pageId = PageTemplateObject->GetIntegerField("pageId");
							PageTemplateData.templateId = PageTemplateObject->GetIntegerField("templateId");
							PageTemplateData.isHideInAem = PageTemplateObject->GetIntegerField("isHideInAem");
							PageTemplateData.isHideInSre = PageTemplateObject->GetIntegerField("isHideInSre");

							PageData.pageTemplates.Add(PageTemplateData);
						}
					}
					result.pages.Add(PageData);
				}
			}
		}
	}
};




#define RABBITMQ_MENU_LIST_REQUEST 10020001
#define RABBITMQ_MENU_LIST_RESPONSE RABBITMQ_MENU_LIST_REQUEST | PROTOCOL_RESP_CMDID_PREF
USTRUCT(BlueprintType)
struct FMQMenuListRequest
{
	GENERATED_BODY()

	FString ToString()
	{
		FString Result;
		Result.Append("{}");
		return Result;
	}
};
USTRUCT(BlueprintType)
struct FMQMenuData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int parentId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int menuId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString menuName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isLeaf;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int MenuLevel;

		//FString ChildMenuStr;
		TArray<FMQMenuData> ChildMenuList;

		void Init(TSharedPtr<FJsonObject> JsonObject)
		{
			parentId = JsonObject->GetIntegerField("parentId");
			menuId = JsonObject->GetIntegerField("menuId");
			menuName = JsonObject->GetStringField("menuName");
			isLeaf = JsonObject->GetBoolField("isLeaf");
			MenuLevel = JsonObject->GetIntegerField("menuLevel");

			TArray<TSharedPtr<FJsonValue>> ChildJsonValueList = JsonObject->GetArrayField("childMenu");
			for (TSharedPtr<FJsonValue> ChildJsonValue : ChildJsonValueList)
			{
				TSharedPtr<FJsonObject> ChildJsonObject = ChildJsonValue->AsObject();
				if (!ChildJsonObject.IsValid())
				{
					continue;
				}

				FMQMenuData ChildMenuData;
				ChildMenuData.Init(ChildJsonObject);
				ChildMenuList.Add(ChildMenuData);
			}
		}
};
USTRUCT(BlueprintType)
struct FMQMenuList
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FMQMenuData> menu;
};
USTRUCT(BlueprintType)
struct FMQMenuListResponse
{
	GENERATED_BODY()

		UPROPERTY(BlueprintReadWrite)
		int status;
	UPROPERTY(BlueprintReadWrite)
		FString message;
	UPROPERTY(BlueprintReadWrite)
		int type;
	UPROPERTY(BlueprintReadWrite)
		int64 fromUuid;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FMQMenuList result;

	void Init(const FString& JsonString)
	{
		if (JsonString.IsEmpty())
		{
			return;
		}

		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef< TJsonReader<TCHAR> > JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonString);

		if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
		{
			status = JsonObject->GetIntegerField("status");
			message = JsonObject->GetStringField("message");
			type = JsonObject->GetIntegerField("type");
			fromUuid = JsonObject->GetIntegerField("fromUuid");

			TArray<FMQMenuData> MenuDataList;

			TSharedPtr<FJsonObject> ResultJsonObject = JsonObject->GetObjectField("result");
			TArray<TSharedPtr<FJsonValue>> MenuJsonValueList = ResultJsonObject->GetArrayField("menu");
			for (TSharedPtr<FJsonValue> MenuJsonValue : MenuJsonValueList)
			{
				TSharedPtr<FJsonObject> MenuJsonObject = MenuJsonValue->AsObject();
				if (!MenuJsonObject.IsValid())
				{
					continue;
				}

				FMQMenuData MenuData;
				MenuData.Init(MenuJsonObject);
				MenuDataList.Add(MenuData);
			}

			FMQMenuList MenuList;
			MenuList.menu = MenuDataList;
			result = MenuList;
		}

	}
};



#define RABBITMQ_MENU_UPDATE_REQUEST 11020001
#define RABBITMQ_MENU_UPDATE_RESPONSE RABBITMQ_MENU_UPDATE_REQUEST | PROTOCOL_RESP_CMDID_PREF
USTRUCT(BlueprintType)
struct FMQMenuUpdateData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int parentId;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int menuId;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString menuName;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool isLeaf;
};
USTRUCT(BlueprintType)
struct FMQMenuUpdateRequest
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int operation;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FMQMenuUpdateData menuNode;


	FString ToString()
	{
		FString Result;
		FJsonObjectConverter::UStructToJsonObjectString(*this, Result);
		return Result;
	}
};
USTRUCT(BlueprintType)
struct FMQMenuUpdateResponse
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		int status;
	UPROPERTY(BlueprintReadWrite)
		FString message;
	UPROPERTY(BlueprintReadWrite)
		int type;
	UPROPERTY(BlueprintReadWrite)
		int64 fromUuid;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FMQMenuUpdateRequest result;
};


#define RABBITMQ_PAGE_UPDATE_REQUEST 11030001
#define RABBITMQ_PAGE_UPDATE_RESPONSE 11030002 | PROTOCOL_RESP_CMDID_PREF
//#define RABBITMQ_PAGE_UPDATE_RESPONSE RABBITMQ_PAGE_UPDATE_REQUEST | PROTOCOL_RESP_CMDID_PREF
USTRUCT(BlueprintType)
struct FMQPageUpdateBaseInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int pageId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int menuId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int sceneId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int uiMaskImage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString pageAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int pageSkyBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString environmentParam;
};
USTRUCT(BlueprintType)
struct FMQComponentData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int64 componentId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int64 parentId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int componentType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString baseInfo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString styleInfo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString queryRule;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString staticData;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString eventInfo;
};
USTRUCT(BlueprintType)
struct FMQPageUpdateComponentNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FMQComponentData component;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FMQPageComponents pageComponent;
};
USTRUCT(BlueprintType)
struct FMQPageUpdateRequest
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FMQPageUpdateBaseInfo baseInfo;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<FMQPageUpdateComponentNode> pageComponentNodes;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<FMQPageTemplates> pageTemplates;

	FString ToString()
	{
		FString Result;
		/*FJsonObjectConverter::UStructToJsonObjectString(*this, Result);
		const FString From("\\");
		const FString To("");
		Result.ReplaceInline(*From, *To);*/

		TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> JsonWriter = TJsonWriterFactory< TCHAR, TPrettyJsonPrintPolicy<TCHAR> >::Create(&Result);
		JsonWriter->WriteObjectStart();

		JsonWriter->WriteObjectStart("baseInfo");
		JsonWriter->WriteValue("pageId", baseInfo.pageId);
		JsonWriter->WriteValue("menuId", baseInfo.menuId);
		JsonWriter->WriteValue("sceneId", baseInfo.sceneId);
		JsonWriter->WriteValue("uiMaskImage", baseInfo.uiMaskImage);
		JsonWriter->WriteValue("pageAnimation", baseInfo.pageAnimation);
		JsonWriter->WriteValue("pageSkyBox", baseInfo.pageSkyBox);
		JsonWriter->WriteValue("environmentParam", baseInfo.environmentParam);
		JsonWriter->WriteObjectEnd();

		JsonWriter->WriteArrayStart("pageComponentNodes");
		for (FMQPageUpdateComponentNode ComponentNode : pageComponentNodes)
		{
			JsonWriter->WriteObjectStart();

			JsonWriter->WriteObjectStart("component");
			JsonWriter->WriteValue("componentId", ComponentNode.component.componentId);
			JsonWriter->WriteValue("parentId", ComponentNode.component.parentId);
			JsonWriter->WriteValue("componentType", ComponentNode.component.componentType);

			JsonWriter->WriteRawJSONValue("baseInfo", ComponentNode.component.baseInfo);
			JsonWriter->WriteRawJSONValue("styleInfo", ComponentNode.component.styleInfo);
			JsonWriter->WriteRawJSONValue("queryRule", ComponentNode.component.queryRule);
			JsonWriter->WriteRawJSONValue("staticData", ComponentNode.component.staticData);
			JsonWriter->WriteRawJSONValue("eventInfo", ComponentNode.component.eventInfo);
			JsonWriter->WriteObjectEnd();

			JsonWriter->WriteObjectStart("pageComponent");
			JsonWriter->WriteValue("pageId", ComponentNode.pageComponent.pageId);
			JsonWriter->WriteValue("componentId", ComponentNode.pageComponent.componentId);
			JsonWriter->WriteValue("displayLevel", ComponentNode.pageComponent.displayLevel);
			JsonWriter->WriteValue("componentFlag", ComponentNode.pageComponent.componentFlag);
			JsonWriter->WriteValue("isHideInAem", ComponentNode.pageComponent.isHideInAem);
			JsonWriter->WriteValue("isHideInSre", ComponentNode.pageComponent.isHideInSre);
			JsonWriter->WriteObjectEnd();

			JsonWriter->WriteObjectEnd();
		}
		JsonWriter->WriteArrayEnd();

		JsonWriter->WriteArrayStart("pageTemplates");
		for (FMQPageTemplates PageTemplate : pageTemplates)
		{
			JsonWriter->WriteObjectStart();
			JsonWriter->WriteValue("pageId", PageTemplate.pageId);
			JsonWriter->WriteValue("templateId", PageTemplate.templateId);
			JsonWriter->WriteValue("isHideInAem", PageTemplate.isHideInAem);
			JsonWriter->WriteValue("isHideInSre", PageTemplate.isHideInSre);
			JsonWriter->WriteObjectEnd();
		}
		JsonWriter->WriteArrayEnd();

		JsonWriter->WriteObjectEnd();
		JsonWriter->Close();

		return Result;
	}
};
USTRUCT(BlueprintType)
struct FMQPageUpdateData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		int operation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FMQPageData page;
};
USTRUCT(BlueprintType)
struct FMQPageUpdateResponse
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		int status;
	UPROPERTY(BlueprintReadWrite)
		FString message;
	UPROPERTY(BlueprintReadWrite)
		int type;
	UPROPERTY(BlueprintReadWrite)
		int64 fromUuid;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FMQPageUpdateData result;
};







#define RABBITMQ_COMPONENT_UPDATE_REQUEST 11060001
#define RABBITMQ_COMPONENT_UPDATE_RESPONSE RABBITMQ_COMPONENT_UPDATE_REQUEST | PROTOCOL_RESP_CMDID_PREF
USTRUCT(BlueprintType)
struct FMQComponentUpdateRequest
{
	GENERATED_BODY()

		UPROPERTY(BlueprintReadWrite)
		int operation;
	UPROPERTY(BlueprintReadWrite)
		TArray<FMQComponentData> components;

	FString ToString()
	{
		FString Result;
		FJsonObjectConverter::UStructToJsonObjectString(*this, Result);
		return Result;
	}
};
USTRUCT(BlueprintType)
struct FMQComponentUpdateResponse
{
	GENERATED_BODY()

		UPROPERTY(BlueprintReadWrite)
		int status;
	UPROPERTY(BlueprintReadWrite)
		FString message;
	UPROPERTY(BlueprintReadWrite)
		int type;
	UPROPERTY(BlueprintReadWrite)
		int64 fromUuid;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FMQMenuUpdateRequest result;
};




#define RABBITMQ_COMPONENT_LIST_REQUEST 10060001
#define RABBITMQ_COMPONENT_LIST_RESPONSE RABBITMQ_COMPONENT_LIST_REQUEST | PROTOCOL_RESP_CMDID_PREF
USTRUCT(BlueprintType)
struct FMQComponentListRequest
{
	GENERATED_BODY()

	FString ToString()
	{
		FString Result;
		Result.Append("{}");
		return Result;
	}
};
USTRUCT(BlueprintType)
struct FMQComponentList
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		TArray<FMQComponentData> ComponentDataList;
};
USTRUCT(BlueprintType)
struct FMQComponentListResponse
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		int status;
	UPROPERTY(BlueprintReadWrite)
		FString message;
	UPROPERTY(BlueprintReadWrite)
		int type;
	UPROPERTY(BlueprintReadWrite)
		int64 fromUuid;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FMQComponentList result;

	void Init(const FString& JsonString)
	{
		if (JsonString.IsEmpty())
		{
			return;
		}

		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef< TJsonReader<TCHAR> > JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonString);

		if (FJsonSerializer::Deserialize(JsonReader, JsonObject, FJsonSerializer::EFlags::StoreNumbersAsStrings))
		{
			status = JsonObject->GetIntegerField("status");
			message = JsonObject->GetStringField("message");
			type = JsonObject->GetIntegerField("type");
			fromUuid = JsonObject->GetIntegerField("fromUuid");

			TSharedPtr<FJsonObject> ResultJsonObject = JsonObject->GetObjectField("result");
			if (!ResultJsonObject.IsValid())
			{
				return;
			}

			TArray<TSharedPtr<FJsonValue>> ComponentList = ResultJsonObject->GetArrayField("components");
			for (TSharedPtr<FJsonValue> ComponentJsonValue : ComponentList)
			{
				TSharedPtr<FJsonObject> ComponentJsonObject = ComponentJsonValue->AsObject();
				if (!ComponentJsonObject.IsValid())
				{
					continue;
				}

				FMQComponentData ComponentData;
				
				FString ComponentId = ComponentJsonObject->GetStringField("componentId");
				ComponentData.componentId = FCString::Atoi64(*ComponentId);
				FString ParentId = ComponentJsonObject->GetStringField("parentId");
				ComponentData.parentId = FCString::Atoi64(*ParentId);
				//ComponentData.parentId = (int64)ComponentJsonObject->GetNumberField("parentId");
				ComponentData.componentType = ComponentJsonObject->GetIntegerField("componentType");

				// base info
				TSharedPtr<FJsonObject> BaseInfoJsonObject = ComponentJsonObject->GetObjectField("baseInfo");
				if (BaseInfoJsonObject.IsValid())
				{
					TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>> > JsonWriter = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&ComponentData.baseInfo);
					FJsonSerializer::Serialize(BaseInfoJsonObject.ToSharedRef(), JsonWriter);
				}

				// style info
				TSharedPtr<FJsonObject> StyleInfoJsonObject = ComponentJsonObject->GetObjectField("styleInfo");
				if (StyleInfoJsonObject.IsValid())
				{
					TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>> > JsonWriter = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&ComponentData.styleInfo);
					FJsonSerializer::Serialize(StyleInfoJsonObject.ToSharedRef(), JsonWriter);
				}

				// query rule
				TSharedPtr<FJsonObject> QueryRuleJsonObject = ComponentJsonObject->GetObjectField("queryRule");
				if (QueryRuleJsonObject.IsValid())
				{
					TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>> > JsonWriter = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&ComponentData.queryRule);
					FJsonSerializer::Serialize(QueryRuleJsonObject.ToSharedRef(), JsonWriter);
				}

				// static data
				TSharedPtr<FJsonObject> StaticDataJsonObject = ComponentJsonObject->GetObjectField("staticData");
				if (StaticDataJsonObject.IsValid())
				{
					TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>> > JsonWriter = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&ComponentData.staticData);
					FJsonSerializer::Serialize(StaticDataJsonObject.ToSharedRef(), JsonWriter);
				}

				// event info
				TSharedPtr<FJsonObject> EventInfoJsonObject = ComponentJsonObject->GetObjectField("eventInfo");
				if (EventInfoJsonObject.IsValid())
				{
					TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>> > JsonWriter = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&ComponentData.eventInfo);
					FJsonSerializer::Serialize(EventInfoJsonObject.ToSharedRef(), JsonWriter);
				}
				result.ComponentDataList.Add(ComponentData);
			}
		}
	}
};




#define RABBITMQ_DATA_SOURCE_TABLE_DATA_REQUEST 10070001
#define RABBITMQ_DATA_SOURCE_TABLE_DATA_RESPONSE RABBITMQ_DATA_SOURCE_TABLE_DATA_REQUEST | PROTOCOL_RESP_CMDID_PREF
USTRUCT(BlueprintType)
struct FMQDataSourceTableDataHeader
{
	GENERATED_BODY()

		UPROPERTY(BlueprintReadWrite)
		FString dataGroup;
	UPROPERTY(BlueprintReadWrite)
		FString dataSource;
	UPROPERTY(BlueprintReadWrite)
		FString filterRuleId;
	UPROPERTY(BlueprintReadWrite)
		int operation = 5;
	UPROPERTY(BlueprintReadWrite)
		int dataOperation = 0;
};
USTRUCT(BlueprintType)
struct FMQDataSourceTableDataRequestBodyColumnsDataRange
{
	GENERATED_BODY()

		UPROPERTY(BlueprintReadWrite)
		int start;
	UPROPERTY(BlueprintReadWrite)
		int end;
};
USTRUCT(BlueprintType)
struct FMQDataSourceTableDataRequestBodyColumnsData
{
	GENERATED_BODY()

		UPROPERTY(BlueprintReadWrite)
		int aggregator;
	UPROPERTY(BlueprintReadWrite)
		FString aliasType;
	UPROPERTY(BlueprintReadWrite)
		int dataType = 0;
	UPROPERTY(BlueprintReadWrite)
		FString filedName;
	UPROPERTY(BlueprintReadWrite)
		FString type;
	UPROPERTY(BlueprintReadWrite)
		FMQDataSourceTableDataRequestBodyColumnsDataRange positionRange;
};
USTRUCT(BlueprintType)
struct FMQDataSourceTableDataRequestBodyCondition
{
	GENERATED_BODY()

		UPROPERTY(BlueprintReadWrite)
		int conditionType;
	UPROPERTY(BlueprintReadWrite)
		TArray<FString> conditionValue;
	UPROPERTY(BlueprintReadWrite)
		int dataType = 0;
	UPROPERTY(BlueprintReadWrite)
		FString filedName;
	UPROPERTY(BlueprintReadWrite)
		FString relation;
	UPROPERTY(BlueprintReadWrite)
		FString type;
};
USTRUCT(BlueprintType)
struct FMQDataSourceTableDataRequestBodyOrder
{
	GENERATED_BODY()

		UPROPERTY(BlueprintReadWrite)
		FString filedName;
	UPROPERTY(BlueprintReadWrite)
		bool descending;
};
USTRUCT(BlueprintType)
struct FMQDataSourceTableDataRequestBody
{
	GENERATED_BODY()

		UPROPERTY(BlueprintReadWrite)
		TArray<FMQDataSourceTableDataRequestBodyColumnsData> columns;
	UPROPERTY(BlueprintReadWrite)
		TArray<FMQDataSourceTableDataRequestBodyCondition> conditions;
	UPROPERTY(BlueprintReadWrite)
		TArray<FMQDataSourceTableDataRequestBodyOrder> orders;
	UPROPERTY(BlueprintReadWrite)
		int rowCount;
};
USTRUCT(BlueprintType)
struct FMQDataSourceTableDataRequest
{
	GENERATED_BODY()

		UPROPERTY(BlueprintReadWrite)
		FMQDataSourceTableDataHeader header;
	UPROPERTY(BlueprintReadWrite)
		FMQDataSourceTableDataRequestBody body;

		FString ToString()
	{
		FString Result;
		//FJsonObjectConverter::UStructToJsonObjectString(*this, Result);

		
		TSharedPtr<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> JsonWriter = TJsonWriterFactory<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>::Create(&Result);
		JsonWriter->WriteObjectStart();

		// header
		JsonWriter->WriteObjectStart("header");

		JsonWriter->WriteValue("dataGroup", header.dataGroup);
		JsonWriter->WriteValue("dataSource", header.dataSource);
		JsonWriter->WriteValue("filterRuleId", header.filterRuleId);
		JsonWriter->WriteValue("operation", header.operation);
		JsonWriter->WriteValue("dataOperation", header.dataOperation);

		JsonWriter->WriteObjectEnd();

		// body
		JsonWriter->WriteObjectStart("body");
		JsonWriter->WriteArrayStart("columns");
		for (FMQDataSourceTableDataRequestBodyColumnsData BodyColumnData : body.columns)
		{
			JsonWriter->WriteObjectStart();
			JsonWriter->WriteValue("aggregator", BodyColumnData.aggregator);
			JsonWriter->WriteValue("aliasType", BodyColumnData.aliasType);
			JsonWriter->WriteValue("dataType", BodyColumnData.dataType);
			JsonWriter->WriteValue("filedName", BodyColumnData.filedName);
			JsonWriter->WriteValue("type", BodyColumnData.type);
			if (BodyColumnData.positionRange.start >= 0 && BodyColumnData.positionRange.end >= 0)
			{
				JsonWriter->WriteObjectStart("positionRange");
				JsonWriter->WriteValue("start", BodyColumnData.positionRange.start);
				JsonWriter->WriteValue("end", BodyColumnData.positionRange.end);
				JsonWriter->WriteObjectEnd();
			}
			JsonWriter->WriteObjectEnd();
		}
		JsonWriter->WriteArrayEnd();

		JsonWriter->WriteArrayStart("conditions");
		for (FMQDataSourceTableDataRequestBodyCondition BodyCondition : body.conditions)
		{
			FString BodyConditionJsonString;
			FJsonObjectConverter::UStructToJsonObjectString(BodyCondition, BodyConditionJsonString);
			JsonWriter->WriteRawJSONValue(BodyConditionJsonString);
		}
		JsonWriter->WriteArrayEnd();
		
		JsonWriter->WriteArrayStart("orders");
		for (FMQDataSourceTableDataRequestBodyOrder BodyOrder : body.orders)
		{
			FString BodyOrderJsonString;
			FJsonObjectConverter::UStructToJsonObjectString(BodyOrder, BodyOrderJsonString);
			JsonWriter->WriteRawJSONValue(BodyOrderJsonString);
		}
		JsonWriter->WriteArrayEnd();

		JsonWriter->WriteValue("rowCount", body.rowCount);

		JsonWriter->WriteObjectEnd();

		JsonWriter->WriteObjectEnd();
		JsonWriter->Close();
		
		return Result;
	}
};
USTRUCT(BlueprintType)
struct FMQDataSourceTableDataResponseBodyFieldAttribute
{
	GENERATED_BODY()

		UPROPERTY(BlueprintReadWrite)
		FString filedName;
	UPROPERTY(BlueprintReadWrite)
		FString type;
	UPROPERTY(BlueprintReadWrite)
		int dataType;
	UPROPERTY(BlueprintReadWrite)
		int aggregator;
	UPROPERTY(BlueprintReadWrite)
		FString aliasType;
	UPROPERTY(BlueprintReadWrite)
		int position;
};
USTRUCT(BlueprintType)
struct FMQDataSourceTableDataResponseBody
{
	GENERATED_BODY()

		UPROPERTY(BlueprintReadWrite)
		TArray<FMQDataSourceTableDataResponseBodyFieldAttribute> fieldAttribute;
	UPROPERTY(BlueprintReadWrite)
		TArray<FTableRowData> rowData;
};
USTRUCT(BlueprintType)
struct FMQDataSourceTableDataResponseResult
{
	GENERATED_BODY()

		UPROPERTY(BlueprintReadWrite)
		FMQDataSourceTableDataHeader header;
	UPROPERTY(BlueprintReadWrite)
		FMQDataSourceTableDataResponseBody body;
};
USTRUCT(BlueprintType)
struct FMQDataSourceTableDataResponse
{
	GENERATED_BODY()

		UPROPERTY(BlueprintReadWrite)
		int status;
	UPROPERTY(BlueprintReadWrite)
		FString message;
	UPROPERTY(BlueprintReadWrite)
		int type;
	UPROPERTY(BlueprintReadWrite)
		int64 fromUuid;
	UPROPERTY(BlueprintReadWrite)
		FMQDataSourceTableDataResponseResult result;

	void Init(const FString& JsonString)
	{
		if (JsonString.IsEmpty())
		{
			return;
		}

		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef< TJsonReader<TCHAR> > JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonString);

		if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
		{
			status = JsonObject->GetIntegerField("status");
			message = JsonObject->GetStringField("message");
			type = JsonObject->GetIntegerField("type");
			fromUuid = JsonObject->GetIntegerField("fromUuid");

			TSharedPtr<FJsonObject> ResultJsonObject = JsonObject->GetObjectField("result");
			if (!ResultJsonObject.IsValid())
			{
				return;
			}

			TSharedPtr<FJsonObject> HeaderJsonObject = ResultJsonObject->GetObjectField("header");
			FMQDataSourceTableDataHeader HeaderStruct;
			FJsonObjectConverter::JsonObjectToUStruct(HeaderJsonObject.ToSharedRef(), &HeaderStruct);

			TSharedPtr<FJsonObject> BodyJsonObject = ResultJsonObject->GetObjectField("body");
			FMQDataSourceTableDataResponseBody BodyStruct;
			if (BodyJsonObject.IsValid())
			{
				TArray<FMQDataSourceTableDataResponseBodyFieldAttribute> AttributeStructArray;
				TArray<TSharedPtr<FJsonValue>> AttributeValueArray = BodyJsonObject->GetArrayField("filedAttribute");
				for (TSharedPtr<FJsonValue> AttributeValue : AttributeValueArray)
				{
					TSharedPtr<FJsonObject> AttributeObject = AttributeValue->AsObject();
					FMQDataSourceTableDataResponseBodyFieldAttribute AttributeStruct;
					FJsonObjectConverter::JsonObjectToUStruct(AttributeObject.ToSharedRef(), &AttributeStruct);
					AttributeStructArray.Add(AttributeStruct);
				}
				BodyStruct.fieldAttribute = AttributeStructArray;

				TArray<TSharedPtr<FJsonValue>> RowDataValueArray = BodyJsonObject->GetArrayField("rowData");
				for (TSharedPtr<FJsonValue> RowDataValue : RowDataValueArray)
				{
					FTableRowData RowDataChild;
					TArray<TSharedPtr<FJsonValue>> RowDataChildValueArray = RowDataValue->AsArray();
					for (TSharedPtr<FJsonValue> RowDataChildValue : RowDataChildValueArray)
					{
						RowDataChild.ColumnDatas.Add(RowDataChildValue->AsString());
					}
					BodyStruct.rowData.Add(RowDataChild);
				}
			}

			result.header = HeaderStruct;
			result.body = BodyStruct;
		}
	}
};



UCLASS()
class MESPRODUCT_API URabbitMqProtocol : public UObject
{
	GENERATED_BODY()


public:
	UFUNCTION(BlueprintCallable)
		static TArray<FMQMenuData> GetMQChildMenuData(FMQMenuData ParentMenu);
	UFUNCTION(BlueprintCallable)
		static FMQLoginResponse ParseMQLoginResponse(FString JsonString);
	UFUNCTION(BlueprintCallable)
		static FMQDataSourceListResponse ParseMQDataSourceListResponse(FString JsonString);
	UFUNCTION(BlueprintCallable)
		static FMQDataSourceColumnInfoResponse ParseMQDataSourceColumnInfoResponse(FString JsonString);
	UFUNCTION(BlueprintCallable)
		static FMQDataSourceColumnDataResponse ParseMQDataSourceColumnDataResponse(FString JsonString);
	UFUNCTION(BlueprintCallable)
		static FMQPageListResponse ParseMQPageListResponse(FString JsonString);
	UFUNCTION(BlueprintCallable)
		static FMQMenuListResponse ParseMQMenuListResponse(FString JsonString);
	UFUNCTION(BlueprintCallable)
		static FMQPageUpdateResponse ParseMQPageUpdateResponse(FString JsonString);
	UFUNCTION(BlueprintCallable)
		static FMQMenuUpdateResponse ParseMQMenuUpdateResponse(FString JsonString);
	UFUNCTION(BlueprintCallable)
		static FMQComponentUpdateResponse ParseMQComponentUpdateResponse(FString JsonString);
	UFUNCTION(BlueprintCallable)
		static FMQComponentListResponse ParseMQComponentListResponse(FString JsonString);
	UFUNCTION(BlueprintCallable)
		static FMQDataSourceTableDataResponse ParseMQDataSourceTableDataResponse(FString JsonString);
};