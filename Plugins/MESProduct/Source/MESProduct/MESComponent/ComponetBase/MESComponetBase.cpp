// Fill out your copyright notice in the Description page of Project Settings.


#include "MESComponetBase.h"
#include "UMG/Public/Components/CanvasPanelSlot.h"
#include "MESProduct/MESComponent/Sqlite/SqliteTool.h"
#include "MESProduct/Tool/MESPathUtility.h"
#include "InputCore/Classes/InputCoreTypes.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "DragParameter.h"
#include "Json.h"
#include "Serialization/JsonSerializer.h"
#include "Components/PanelWidget.h"
#include "MESGlobalData.h"
#include "MESProduct/Tool/MESPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "MESProduct/MESComponent/RabbitMq/Public/RabbitMqClient.h"
#include "MESProduct/MESComponent/RabbitMq/Public/RabbitMqProtocol.h"
#include "MESProduct/Tool/MESBlueprintFunctionLibrary.h"
#include "MESProduct/Tool/MESGameInstance.h"
#include "String.h"
#include "Kismet/KismetArrayLibrary.h"
#include "Kismet/BlueprintMapLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "UObject/UnrealType.h"
#include "GenericPlatform/GenericPlatformMemory.h"

#define LOCTEXT_NAMESPACE "MECharts"

TArray<UMESComponetBase*> UMESComponetBase::ComponetArray = TArray<UMESComponetBase*>();

TMap<int, UMESComponetBase*> UMESComponetBase::DontDestroyMap = TMap<int, UMESComponetBase*>();

FSelectComponetDelegate UMESComponetBase::SelectComponetEvent = FSelectComponetDelegate();
UUserWidget* UMESComponetBase::SelectWidget;

void UMESComponetBase::HttpRequest(FString url, FString Methond)
{
	FHttpModule* HttpModule = &FHttpModule::Get();
#if ENGINE_MINOR_VERSION >= 26
    	TSharedRef<IHttpRequest,ESPMode::ThreadSafe> HttpRequest = HttpModule->CreateRequest();
#else
    	TSharedRef<IHttpRequest> HttpRequest = HttpModule->CreateRequest();
#endif
	HttpRequest->SetVerb(Methond);
	HttpRequest->SetURL(url);
	UE_LOG(LogTemp, Log, TEXT("get url: %s"), *url);
	HttpRequest->SetHeader("Content-Type", "application/json");
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UMESComponetBase::OnRequestComplete);
	HttpRequest->ProcessRequest();
}

void UMESComponetBase::OnRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
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
		HandleJson(Result);
		UE_LOG(LogTemp, Warning, TEXT("received http response: %s"), *Result);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("received http response error: %d"), HttpResponse->GetResponseCode());
	}
}

void UMESComponetBase::HandleJson(FString Json)
{
	//反向解析Json数据变成TableData
	
	TSharedRef< TJsonReader<> > Reader = TJsonReaderFactory<>::Create(Json);
	TSharedPtr<FJsonValue> JsonParsed;
	bool bSuccessful = FJsonSerializer::Deserialize(Reader, JsonParsed);
	if (bSuccessful)
	{
		TSharedPtr<FJsonObject> jsonMember = JsonParsed->AsObject();
		if (jsonMember->HasField("code"))
		{
			int code = jsonMember->GetIntegerField(TEXT("code"));
			if (code < 0)
			{
				FString Message;
				if (jsonMember->TryGetStringField(TEXT("message"),Message))
				{
					UE_LOG(LogTemp, Warning, TEXT("erro message "),*Message);
				}
				return;
			}
		}
		TableData.Headers.Empty();
		TableData.RowDatas.Empty();
		TArray <FTableRowData> TemRowDatas;
		if (jsonMember->HasField("data"))
		{
			TArray<TSharedPtr<FJsonValue>> dataValue = jsonMember->GetArrayField(TEXT("data"));
			for (int32 i = 0; i < dataValue.Num(); ++i)
			{
				TSharedPtr<FJsonObject> childData = (dataValue)[i]->AsObject();
				if (i == 0)
				{
					int index = 0;
					for (TPair<FString, TSharedPtr<FJsonValue>>& element : childData->Values)
					{
						FTableFiled tableFiled = FTableFiled();
						tableFiled.FieldIndex = index++;
						tableFiled.FieldName = element.Key;
						tableFiled.HeaderType = "string";
						TableData.Headers.Add(tableFiled);
					}
				}
				FTableRowData rowData = FTableRowData();
				for (TPair<FString, TSharedPtr<FJsonValue>>& element : childData->Values)
				{
					rowData.ColumnDatas.Add(element.Value->AsString());
				}
				//TableData.RowDatas.Add(rowData);
				TemRowDatas.Add(rowData);
			}
			//如果有数据，就创建筛选数组保存要移除的下标
			TArray<int32> FilterIndex;

			//计算哪些数据需要删除,把要删除的下标放入FilterIndex
			for (int32 i = 0; i < FiterRules.Num(); i++)
			{
				for (int32 j = 0; j < TableData.Headers.Num(); j++)
				{
					if (FiterRules[i].ColumnName == TableData.Headers[j].FieldName)
					{
						FFilterRule f = FiterRules[i];
						float A = FCString::Atof(*(f.FilterValue));
						//UKismetSystemLibrary::PrintString(GetWorld(), FString::SanitizeFloat(TemRowDatas[j].ColumnDatas.Num()), true,true, FLinearColor(1.0,0.0,1.0), 100.0f);
						for (int k = 0; k < TemRowDatas.Num(); k++)
						{
							
							float B = FCString::Atof(*(TemRowDatas[k].ColumnDatas[j]));
							switch (f.FilterType)
							{
							case EFilterType::greater:// >
								if (B <= A)
									FilterIndex.AddUnique(k);
								break;
							case EFilterType::equal:// =
								if (B != A)
									FilterIndex.AddUnique(k);
								break;
							case EFilterType::greater_equal:// >=
								if (B < A)
									FilterIndex.AddUnique(k);
								break;
							case EFilterType::less: // <
								if (B >= A)
									FilterIndex.AddUnique(k);
								break;
							case EFilterType::less_equal:// <=
								if (B > A)
									FilterIndex.AddUnique(k);
								break;
							case EFilterType::in:
								if(!TemRowDatas[k].ColumnDatas[j].Contains(FiterRules[i].FilterValue))
									FilterIndex.AddUnique(k);
								break;
							default:
								if (B != A)
								{
									FilterIndex.AddUnique(k);
								}
								break;
							}
						}
					}
				}
			}
			//把要移除的数据删除	
			for (int32 i = 0; i < FilterIndex.Num(); i++)
			{
				int index = FilterIndex[i];
				TemRowDatas.RemoveAt(index);
				for (int k = i+1; k < FilterIndex.Num(); k++)
					FilterIndex[k] = FilterIndex[k] - 1;
			}
			
			//i小于2，是因为只有升序或降序
			for (int32 i = 0; i < Sorts.Num() && i<2 ; i++)
			{

				//找到排序的列下标
				for (int32 j = 0; j < TableData.Headers.Num(); j++)
				{
					if (Sorts[i].ColumnName == TableData.Headers[j].FieldName)
					{
						//把找到的列转换为float数据存入，key是下标，value是数据
						TMap<int32, float> TemSorts;
						//UKismetSystemLibrary::PrintString(GetWorld(), Sorts[i].ColumnName, true, true, FLinearColor(1.0, 1.0, 0.0), 100.0f);
						int32 ShouIndex = 0;
						if(i == 0)
							ShouIndex = j;

						bool XunHuan = true;
						int32 XunChaIndex = 0;
						
						while (XunHuan && XunChaIndex < TemRowDatas.Num())
						{
							//循环获取列数据
							if(i == 0)
								for (int32 k = 0; k < TemRowDatas.Num(); k++)
								{
									//UKismetSystemLibrary::PrintString(GetWorld(), TemRowDatas[k].ColumnDatas[j], true, true, FLinearColor(1.0, 1.0, 0.0), 100.0f);
									float ftmp = FCString::Atof(*(TemRowDatas[k].ColumnDatas[j]));
									TemSorts.Add(k, ftmp);
								}
							else
							{
								FString Stmp = TemRowDatas[XunChaIndex].ColumnDatas[ShouIndex];
								for (; XunChaIndex < TemRowDatas.Num(); XunChaIndex++)
								{
									if (Stmp == TemRowDatas[XunChaIndex].ColumnDatas[ShouIndex])
									{
										//UKismetSystemLibrary::PrintString(GetWorld(), TemRowDatas[k].ColumnDatas[j], true, true, FLinearColor(1.0, 1.0, 0.0), 100.0f);
										float ftmp = FCString::Atof(*(TemRowDatas[XunChaIndex].ColumnDatas[j]));
										TemSorts.Add(XunChaIndex, ftmp);
									}
									else
									{
										XunChaIndex++;
										break;
									}
								}
							}
							//
							if(TemSorts.Num() > 1)
							{
								//排序，key是下标
								switch (Sorts[i].SortType)
								{
								case EDataSortType::Up:
									TemSorts.ValueSort([](const float& A, const float& B) {
										return A < B;
									});
									break;
								case EDataSortType::Down:
									TemSorts.ValueSort([](const float& A, const float& B) {
										return A > B;
									});
									break;
								}

								//获取排序后的keys
								TArray<int32> Index;
								TemSorts.GetKeys(Index);

								TArray <FTableRowData> TemRowDatass;
								TemRowDatass = TemRowDatas;
								for (int k = 0; k < Index.Num(); k++)
								{
									//TemRowDatas[k].ColumnDatas = *TemRowDatass.FindByKey(Index[k]).ColumnDatas;
									//TemRowDatass.Add(TemRowDatas[k]);
									TemRowDatas[k] = TemRowDatass[(Index[k])];
								
								}
							}
							TemSorts.Empty();
							if (i == 0 || XunChaIndex > TemRowDatas.Num())
								XunHuan = false;
								
						}
							//TemRowDatas = TemRowDatass;
						break;
					}	
				}
			}
			//放入存储的数据		
			TableData.RowDatas.Append(TemRowDatas);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("no Data "));
		}
		UpdateDataCallBack.Broadcast(TableData);
		DataUpdate();
	}
}

void UMESComponetBase::GetComponetData()
{
	
	if (DataSourceType == EDataSourceType::NONE)
	{
		UpdateDataCallBack.Broadcast(TableData);
		return;
	}
	UMESGameInstance* GameInstance = UMESGameInstance::Get(GetWorld());
	if (DataSourceType == EDataSourceType::SQLite)
	{
		FString SQL = TEXT("Select rowid as UID,* from '") + TableName + "'";
		FString FilterTypeStr = " = ";
		for (int i = 0; i < FiterRules.Num(); i++)
		{
			FFilterRule f = FiterRules[i];
			switch (f.FilterType)
			{
			case EFilterType::greater:
				FilterTypeStr = TEXT(" > ");
				break;
			case EFilterType::equal:
				FilterTypeStr = TEXT(" = ");
				break;
			case EFilterType::greater_equal:
				FilterTypeStr = TEXT(" >= ");
				break;
			case EFilterType::less:
				FilterTypeStr = TEXT(" < ");
				break;
			case EFilterType::less_equal:
				FilterTypeStr = TEXT(" <= ");
				break;
			case EFilterType::in:
				FilterTypeStr = TEXT(" in ");
				break;
			default:
				FilterTypeStr = TEXT(" = ");
				break;
			}
			if (i == 0)
			{
				SQL += TEXT(" WHERE ") + f.ColumnName + FilterTypeStr + TEXT("'") + f.FilterValue + TEXT("' ");
			}
			else
			{
				if (IsAND)
				{
					SQL += TEXT(" AND ") + f.ColumnName + FilterTypeStr + TEXT("'") + f.FilterValue + TEXT("' ");
				}
				else
				{
					SQL += TEXT(" OR ") + f.ColumnName + FilterTypeStr + TEXT("'") + f.FilterValue + TEXT("' ");
				}
			}
		}

		for (int i = 0; i < Sorts.Num(); i++)
		{
			FDataSort f = Sorts[i];
			switch (f.SortType)
			{
			case EDataSortType::Up:
				FilterTypeStr = TEXT(" ASC ");
				break;
			case EDataSortType::Down:
				FilterTypeStr = TEXT(" DESC ");
				break;
			}
			if (i == 0)
			{
				SQL += TEXT(" ORDER BY ") + f.ColumnName + FilterTypeStr;
			}
			else
			{
				SQL += TEXT(", ") + f.ColumnName + FilterTypeStr;
			}
		}
		SQL += " LIMIT " + FString::FromInt(MaxNum);
		FString DBPath = UMESPathUtility::GetProductBasePath() + TEXT("数据\\") + DataGroup + TEXT(".db");
		if (GameInstance != nullptr) {
			if (GameInstance->IsProjectOffline)
			{
				DBPath = UMESPathUtility::GetStandaloneProjectDataPath(GameInstance->ProjectInfo_Offline.ProjectName) + DataGroup + TEXT(".db");
			}
			else
			{
				DBPath = UMESPathUtility::GetServerProjectDataPath(GameInstance->ProjectInfo_Server.projectName) + DataGroup + TEXT(".db");
			}
		}
		USqliteTool::LoadTableData(DBPath,SQL, TableName, TableData);
		UpdateDataCallBack.Broadcast(TableData);
		DataUpdate();
		return;
	}
	if (DataSourceType == EDataSourceType::API)
	{
		//TODO 后面需要根据SQLite 获取URL
		FString SQL = TEXT("Select API_URL from SDC_API where API_GroupName='") + DataGroup + TEXT("' AND API_InterfaceName='") + TableName + TEXT("'");
		FTableData Result;
		FString DBPath = UMESPathUtility::GetProductBasePath() + TEXT("数据\\配置数据库.db");
		if (GameInstance != nullptr) {
			DBPath = UMESPathUtility::GetStandaloneProjectDBPath(GameInstance->ProjectInfo_Offline.ProjectName);
		}
		USqliteTool::LoadTableData(DBPath, SQL, TEXT("SDC_API"), Result);
		if (Result.RowDatas.Num() > 0 && Result.RowDatas[0].ColumnDatas.Num() > 0)
		{//数据存在的时候才 允许 访问
			FString url = Result.RowDatas[0].ColumnDatas[0];
			//FString url = TEXT("http://10.0.8.30:18560/me/public/rs/api/data/business?table=GDP%E6%83%85%E5%86%B5&limit=10");
			//然后根据URL 获取Json数据
			HttpRequest(url, "GET");
		}
		return;
	}
	if (DataSourceType == EDataSourceType::DAC)
	{
		AMESPlayerController* PlayerController = Cast<AMESPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		if (PlayerController && PlayerController->IsValidLowLevelFast())
		{
			URabbitMqClient* RabbitMqComponent = PlayerController->RabbitMqComponent;
			if (RabbitMqComponent)
			{
				FMQDataSourceColumnInfoRequest ColumnInfoRequest;
				ColumnInfoRequest.dataGroup = DataGroup;
				ColumnInfoRequest.dataSource = TableName;
				FMqResponseCallback Callback;
				Callback.BindUFunction(this, "OnDACColumnInfoResponse");
				RabbitMqComponent->DataSourceColumnInfoRequest(ColumnInfoRequest, Callback);
			}
		}
		return;
	}
}

void UMESComponetBase::SetFinish()
{
	//赋值坐标
	CanDrag = true;
	UCanvasPanelSlot* s = Cast<UCanvasPanelSlot>(Slot);
	if (s != nullptr)
	{
		if (s->GetAnchors() == FAnchors())
		{
			s->SetPosition(CommonData.Pos);
			s->SetSize(CommonData.Size);
			s->SetZOrder(CommonData.Depth);
		}
		else
		{
			if (SelectWidget != nullptr)
			{
				UCanvasPanelSlot* ps = Cast<UCanvasPanelSlot>(SelectWidget->Slot);
				if (ps != nullptr)
				{
					ps->SetPosition(CommonData.Pos);
					ps->SetSize(CommonData.Size);
					ps->SetZOrder(CommonData.Depth);
				}
			}
		}
		if (BgImage != nullptr)
		{
			BgImage->SetOpacity(CommonData.BgAlpha);
		}
		if (CommonData.Visible)
		{
			SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			SetVisibility(ESlateVisibility::Hidden);
		}
	}
	//获取组件数据
	GetComponetData();
}

void UMESComponetBase::SetShow(bool isVisible)
{
	CommonData.Visible = isVisible;
	if (isVisible == true)
	{
		SetVisibility(ESlateVisibility::Visible);
		return;
	}
	else
	{
		SetVisibility(ESlateVisibility::Hidden);
	}
}

bool UMESComponetBase::GetIsSelected()
{
	UCanvasPanelSlot* s = Cast<UCanvasPanelSlot>(Slot);
	if (s != nullptr)
	{
		return s->GetAnchors() != FAnchors();
	}
	return false;
}

void UMESComponetBase::GenUUID()
{
	CommonData.UUID = FDateTime::Now().GetTicks();//后期需要叠加 PageID来保证全局唯一
}

void UMESComponetBase::SetPosAndSize(FVector2D Pos, FVector2D Size)
{
	CommonData.Pos = Pos;
	CommonData.Size = Size;
}

FColor UMESComponetBase::GetColorByIndex(int index)
{
	if (CommonData.ColorIndex >= Colors.Num() || CommonData.ColorIndex < 0 || index < 0)
	{
		return FColor::Black;
	}
	if (Colors[CommonData.ColorIndex].ColorList.Num() == 0)
	{
		return FColor::Black;
	}
	int realIndex = index % Colors[CommonData.ColorIndex].ColorList.Num();
	return Colors[CommonData.ColorIndex].ColorList[realIndex];
}

void UMESComponetBase::SetBgAlpha(float alpha)
{
	CommonData.BgAlpha = alpha;
	if (BgImage != nullptr)
	{
		BgImage->SetOpacity(CommonData.BgAlpha);
	}
}

void UMESComponetBase::SetDepth(int NewDepth)
{
	CommonData.Depth = NewDepth;
	UCanvasPanelSlot* s = Cast<UCanvasPanelSlot>(Slot);
	if (s != nullptr)
	{
		if (s->GetAnchors() == FAnchors())
		{
			s->SetZOrder(CommonData.Depth);
		}
		else
		{
			if (SelectWidget != nullptr)
			{
				UCanvasPanelSlot* ps = Cast<UCanvasPanelSlot>(SelectWidget->Slot);
				if (ps != nullptr)
				{
					ps->SetZOrder(CommonData.Depth);
				}
			}
		}
	}
}

void UMESComponetBase::SetSelectColorIndex(int SelectColorIndex)
{
	CommonData.ColorIndex = SelectColorIndex;
}

TArray<FString> UMESComponetBase::GetDataColoumn(FString FieldName)
{
	int Min = 0;
	int Max = 0;
	TArray<FString> result;
	TArray<int> Indexs = GetDataColoumnIndex(FieldName);
	if (Indexs.Num() == 0)
	{
		return result;
	}
	for (int i = 0; i < TableData.RowDatas.Num();i++)
	{
		result.Add(TableData.RowDatas[i].ColumnDatas[Indexs[0]]);
	}
	return result;
}

TArray<FTableRowData> UMESComponetBase::GetDataColoumns(FString FieldName)
{
	TArray<FTableRowData> result;
	int Min = 0;
	int Max = 0;
	TArray<int> Indexs = GetDataColoumnIndex(FieldName);
	for (int i = 0; i < Indexs.Num();i++)
	{
		FTableRowData rowData;
		for (int j = 0; j < TableData.RowDatas.Num(); j++)
		{
			rowData.ColumnDatas.Add(TableData.RowDatas[j].ColumnDatas[Indexs[i]]);
		}
		rowData.HeaderName = TableData.Headers[Indexs[i]].FieldName;
		result.Add(rowData);
	}
	return result;
}

void UMESComponetBase::SetParent(int64 UUID)
{
	CommonData.ParentUUID = UUID;
}

UMESComponetBase* UMESComponetBase::GetDontDestroyComponet(int StyleID)
{
	if (DontDestroyMap.Contains(StyleID))
	{
		return DontDestroyMap[StyleID];
	}
	return nullptr;
}

TArray<int> UMESComponetBase::GetDataColoumnIndex(FString FieldName)
{
	TArray<int> result;
	if (!DataBinds.Contains(FieldName))
	{
		return result;
	}
	int index = -1;
	FDataBind f = DataBinds[FieldName];
	if (f.BindType == EDataBindType::FiledName)
	{
		index = TableData.Headers.IndexOfByPredicate([=](const FTableFiled tf) {return tf.FieldName == f.FilterValue; });
		if (index == -1)return result;
		result.Add(index);
		return result;
	}
	if (f.BindType == EDataBindType::FiledIndex)
	{
		int filter = FCString::Atoi(*f.FilterValue);
		index = TableData.Headers.IndexOfByPredicate([=](const FTableFiled tf) {return tf.FieldIndex == filter; });
		if (index == -1)return result;
		result.Add(index);
		return result;
	}
	if (f.BindType == EDataBindType::FiledNameList)
	{
		TArray<FString> splitStr;
		f.FilterValue.ParseIntoArray(splitStr, TEXT(";"), true);
		for (int j = 0; j< splitStr.Num(); j++)
		{
			index = TableData.Headers.IndexOfByPredicate([=](const FTableFiled tf) {return tf.FieldName == splitStr[j]; });
			if (index != -1)
			{
				result.Add(index);
			}			
		}
		return result;
	}
	if (f.BindType == EDataBindType::FiledArea)
	{
		TArray<FString> splitStr;
		f.FilterValue.ParseIntoArray(splitStr, TEXT(","), true);
		if (splitStr.Num() != 2)
		{
			return result;
		}
		int Min = FCString::Atoi(*splitStr[0]);
		int Max = FCString::Atoi(*splitStr[1]);
		if (Min < 0)Min = 0;
		if (Max < 0)Max = 0;
		if (Min >= TableData.Headers.Num())
		{
			Min = TableData.Headers.Num() - 1;
		}
		if (Max >= TableData.Headers.Num())
		{
			Max = TableData.Headers.Num() - 1;
		}
		if (Max < Min) {
			int temp = Min;
			Min = Max;
			Max = temp;
		}
		for (int i = Min; i <= Max; i++)
		{
			index = TableData.Headers.IndexOfByPredicate([=](const FTableFiled tf) {return tf.FieldIndex == i; });
			if (index != -1)
			{
				result.Add(index);
			}
		}
		return result;
	}
	return result;
}

void UMESComponetBase::CopyPropertyFromAnother(UMESComponetBase* Left)
{
	if (Left == nullptr)return;
	int TempStyleID = CommonData.StyleID;
	CommonData = Left->CommonData;
	CommonData.StyleID = TempStyleID;
	DataSourceType = Left->DataSourceType;
	DataGroup = Left->DataGroup;
	TableName = Left->TableName;
	CustomConfig = Left->CustomConfig;
	Events = Left->Events;
	DataBinds = Left->DataBinds;
	FiterRules = Left->FiterRules;
	Sorts = Left->Sorts;
	MaxNum = Left->MaxNum;

	IsSelected = false;
	CanDrag = Left->CanDrag;
	TableData = Left->TableData;
	CanConstruct = true;
}

void UMESComponetBase::SetSelectUserWidget(UUserWidget* Widget)
{
	SelectWidget = Widget;
}

int64 UMESComponetBase::GetFilterRuleId()
{
	return CommonData.UUID;
}

UMESComponetData* UMESComponetBase::ConvertMESComponetData()
{
	UMESComponetData* res = NewObject<UMESComponetData>();
	res->DataSourceType = DataSourceType;
	res->DataGroup = DataGroup;
	res->TableName = TableName;
	res->DataBinds = DataBinds;
	res->FiterRules = FiterRules;
	res->Sorts = Sorts;
	res->MaxNum = MaxNum;
	res->TableData = TableData;
	res->UIBase = this;
	return res;
}

void UMESComponetBase::SetMESComponetData(UMESComponetData* data)
{
	DataSourceType = data->DataSourceType;
	DataGroup = data->DataGroup;
	TableName = data->TableName;
	DataBinds = data->DataBinds;
	FiterRules = data->FiterRules;
	Sorts = data->Sorts;
	MaxNum = data->MaxNum;
	TableData = data->TableData;
}

UMESComponetBase* UMESComponetBase::GetUIComponetByUUID(int64 UUID)
{
	for (int i = 0; i < ComponetArray.Num(); i++)
	{
		if (ComponetArray[i]->CommonData.UUID == UUID)
		{
			return ComponetArray[i];
		}
	}
	return nullptr;
}

void UMESComponetBase::SetComponetData(TArray<FString> SQLText)
{
	if (DataSourceType == EDataSourceType::SQLite)
	{
		USqliteTool::ExecuteSqlString(UMESPathUtility::GetProductBasePath() + TEXT("数据\\") + DataGroup + TEXT(".db"),SQLText);
	}
	return;
}

void UMESComponetBase::BindGetComponetData(FUpdateDataSingleDelegate CallBack)
{
	if (!UpdateDataCallBack.Contains(CallBack))
	{
		UpdateDataCallBack.Add(CallBack);
	}
	GetComponetData();
}

void UMESComponetBase::UnBindGetComponetData(FUpdateDataSingleDelegate CallBack)
{
	UpdateDataCallBack.Remove(CallBack);
}

void UMESComponetBase::OnClick()
{
	IsForceDown = true;
	FGeometry Gemo = GetCachedGeometry();
	FPointerEvent pointerEvent;
	NativeOnMouseButtonDown(Gemo, pointerEvent);
	NativeOnMouseButtonUp(Gemo, pointerEvent);
	IsForceDown = false;
}

UMESComponetBase* UMESComponetBase::Clone()
{
	UMESComponetBase* CloneObj = CreateWidget<UMESComponetBase>(GetWorld(), this->GetClass());
	//TODO 后去需要做一个 属性生成 拷贝的 ，有可能做不了。因为有可能 没法反射，没法实现完整的 拷贝
	return CloneObj;
}

void UMESComponetBase::BindSelectEvent(FSelectSingleDelegate SelectEvent)
{
	SelectComponetEvent.Add(SelectEvent);
}

void UMESComponetBase::UnBindSelectEvent(FSelectSingleDelegate SelectEvent)
{
	SelectComponetEvent.Remove(SelectEvent);
}

void UMESComponetBase::SynchronizeProperties()
{
	Super::SynchronizeProperties();
}

void UMESComponetBase::PreSave(const class ITargetPlatform* TargetPlatform)
{
#if WITH_EDITORONLY_DATA
	CommonData.Size = DesignTimeSize;
	Super::PreSave(TargetPlatform);
#endif
}

FReply UMESComponetBase::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (!IsForceDown)
	{
		if (!UMESGlobalData::IsDesignMode || InMouseEvent.GetEffectingButton() != EKeys::LeftMouseButton || !CanDrag)
		{
			return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
		}
	}
	if (GetOwningPlayer()->IsInputKeyDown(EKeys::LeftControl)) {
		IsSelected = false;
	}
	else
	{
		for (int i = 0; i < ComponetArray.Num(); i++)
		{
			if (ComponetArray[i] == this)
			{
				ComponetArray[i]->IsSelected = true;
			}
			else
			{
				ComponetArray[i]->IsSelected = false;
			}
		}
	}
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	SelectComponetEvent.Broadcast(this);
	return FReply::Handled();
}

void UMESComponetBase::NativePreConstruct()
{
	if (IsDesignTime())
	{
		Super::NativePreConstruct();
		return;
	}
	ComponetArray.Add(this);
	if (DontDestroy)
	{
		if (!DontDestroyMap.Contains(CommonData.StyleID))
		{
			DontDestroyMap.Add(CommonData.StyleID, this);
		}
		else
		{
			DontDestroy = false;
		}
	}
	if (CanConstruct)
	{
		Super::NativePreConstruct();
	}
	CanConstruct = false;
}

void UMESComponetBase::NativeDestruct()
{
	IsSelected = false;
	ComponetArray.Remove(this);
	if (DontDestroy)
	{
		DontDestroyMap.Remove(CommonData.StyleID);
	}

	AMESPlayerController* PlayerController = Cast<AMESPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PlayerController && PlayerController->IsValidLowLevelFast())
	{
		URabbitMqClient* RabbitMqComponent = PlayerController->RabbitMqComponent;
		if (RabbitMqComponent)
		{
			RabbitMqComponent->UnregisterMessage(RABBITMQ_DATA_SOURCE_COLUMN_INFO_RESPONSE, this);
			RabbitMqComponent->UnregisterMessage(RABBITMQ_DATA_SOURCE_TABLE_DATA_RESPONSE, this);

			if (DataSourceType == EDataSourceType::DAC)
			{
				FMQDataSourceTableDataRequest TableRequest;
		
				TableRequest.header.dataGroup = DataGroup;
				TableRequest.header.dataSource = TableName;
				UMESBlueprintFunctionLibrary::Int64ToString(GetFilterRuleId(), TableRequest.header.filterRuleId);
				TableRequest.header.operation = 2;
				TableRequest.header.dataOperation = 0;

				RabbitMqComponent->Request(10070001,TableRequest.ToString());
			}
		}
	}
	Super::NativeDestruct();
}


void UMESComponetBase::ParseEvent(UObject* InnerObject, FString FunctionName, FString Param)
{
	FName const FunctionFName(*FunctionName);

	UObject* Caller = this;
	UFunction * Function = FindFunction(FunctionFName);

	if (!Function)
	{
		UE_LOG(LogTemp, Warning, TEXT("Parse Event has no function found,please check the sender and receiver"))
		return;
	}

	if (Function->NumParms != 2)
	{
		UE_LOG(LogTemp, Warning, TEXT("Event Param count error,please check"))
		return;
	}

	uint8 * Buffer = (uint8*)FMemory_Alloca(Function->ParmsSize);
	FMemory::Memzero(Buffer, Function->ParmsSize);
	int32 index = 0;
	for (TFieldIterator<UProperty> It(Function); It && It->HasAnyPropertyFlags(CPF_Parm); ++It) {
		UProperty* FunctionProperty = *It;
		index++;
		FString Type = FunctionProperty->GetCPPType();
		FString Name = FunctionProperty->GetName();

		if (index == 1)
		{
			if (Type != "UObject*" || Name != "Sender") return;
			*FunctionProperty->ContainerPtrToValuePtr<UObject*>(Buffer) = InnerObject;
		}
		else if (index == 2)
		{
			if (Type != "FString" || Name != "Param") return;
			*FunctionProperty->ContainerPtrToValuePtr<FString>(Buffer) = Param;
		}
	}
	Caller->ProcessEvent(Function, Buffer);
}

TSharedRef<SWidget> UMESComponetBase::RebuildWidget()
{
	if (IsDesignTime())
	{
#if WITH_EDITORONLY_DATA
		CommonData.Size = DesignTimeSize;
#endif
	}
	return Super::RebuildWidget();
}

void UMESComponetBase::OnObjectModified(UObject* Object)
{

}

#if WITH_EDITOR

#include "UMGStyle.h"

const FText UMESComponetBase::GetPaletteCategory()
{
	return LOCTEXT("MEChartsPaletteCategory", "MESComponet");
}

#endif

void UMESComponetBase::OnDACColumnInfoResponse(FString JsonString)
{
	FMQDataSourceColumnInfoResponse Response = URabbitMqProtocol::ParseMQDataSourceColumnInfoResponse(JsonString);
	// get table header info and request table data.
	FString ResponseDataGroup = Response.result.dataGroup;
	FString ResponseDataSource = Response.result.dataSource;
	if (ResponseDataGroup != DataGroup || ResponseDataSource != TableName)
	{
		return;
	}

	TArray<FMQDataSourceColumnInfoItem> ResponseColumns = Response.result.columns;

	AMESPlayerController* PlayerController = Cast<AMESPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PlayerController && PlayerController->IsValidLowLevelFast())
	{
		URabbitMqClient* RabbitMqComponent = PlayerController->RabbitMqComponent;
		if (RabbitMqComponent)
		{
			FMQDataSourceTableDataRequest TableRequest;
			//request.header
			TableRequest.header.dataGroup = DataGroup;
			TableRequest.header.dataSource = TableName;
			//TableRequest.header.filterRuleId = FString::FromInt(GetFilterRuleId());
			UMESBlueprintFunctionLibrary::Int64ToString(GetFilterRuleId(), TableRequest.header.filterRuleId);
			TableRequest.header.operation = 5;
			TableRequest.header.dataOperation = 0;

			//request.body
			TableRequest.body.rowCount = MaxNum;

			struct LocalFind
			{
				static FMQDataSourceColumnInfoItem* FindItem(TArray<FMQDataSourceColumnInfoItem>& ItemArray, const FString& FieldName)
				{
					return ItemArray.FindByPredicate([=](FMQDataSourceColumnInfoItem& Item) {
						return Item.fieldName == FieldName;
					});
				}
			};
			// request.body.columns
			if (DataBinds.Num() > 0)
			{
				for (auto DataBindInfo : DataBinds)
				{
					FString FieldName = DataBindInfo.Key;
					FDataBind DataBind = DataBindInfo.Value;

					switch (DataBind.BindType)
					{
						case EDataBindType::FiledArea:
						{
							TArray<FString> FilterValueArray;
							FString CleanFilterValue = DataBind.FilterValue.Replace(TEXT("，"), TEXT(","));
							CleanFilterValue.ParseIntoArray(FilterValueArray, TEXT(","), true);
							
							int PositionRangeStart = -1;
							int PositionRangeEnd = 0;
							for (FString FilterValue : FilterValueArray)
							{
								int RangeValue = FCString::Atoi(*FilterValue);
								if (PositionRangeStart == -1)
								{
									PositionRangeStart = RangeValue;
								}
								PositionRangeStart = FMath::Min(PositionRangeStart, RangeValue);
								PositionRangeEnd = FMath::Max(PositionRangeEnd, RangeValue);
							}

							FMQDataSourceTableDataRequestBodyColumnsData ColumnData;
							ColumnData.aggregator = 1;
							ColumnData.aliasType = TEXT("字段");
							ColumnData.type = "null";
							ColumnData.positionRange.start = PositionRangeStart;
							ColumnData.positionRange.end = PositionRangeEnd;
							TableRequest.body.columns.Add(ColumnData);
						}
						break;

						case EDataBindType::FiledIndex:
						{
							int FilterIndex = FCString::Atoi(*DataBind.FilterValue);
							if (FilterIndex >= ResponseColumns.Num())
							{
								break;
							}

							FMQDataSourceTableDataRequestBodyColumnsData ColumnData;
							ColumnData.aggregator = 1;
							ColumnData.aliasType = TEXT("字段");
							ColumnData.type = "null";
							ColumnData.positionRange.start = FilterIndex;
							ColumnData.positionRange.end = FilterIndex;
							TableRequest.body.columns.Add(ColumnData);
						}
						break;

						case EDataBindType::FiledName:
						{
							FMQDataSourceTableDataRequestBodyColumnsData ColumnData;
							ColumnData.aggregator = 1;
							ColumnData.aliasType = TEXT("字段");

							FMQDataSourceColumnInfoItem* ColumnInfoItem = LocalFind::FindItem(ResponseColumns, DataBind.FilterValue);
							if (ColumnInfoItem)
							{
								ColumnData.filedName = ColumnInfoItem->fieldName;
								ColumnData.dataType = FCString::Atoi(*ColumnInfoItem->dataType);
								//ColumnData.type = ColumnInfoItem->type;
								ColumnData.type = "DIMENSION";
								ColumnData.positionRange.start = -1;
								ColumnData.positionRange.end = -1;

								TableRequest.body.columns.Add(ColumnData);
							}
						}
						break;

						case EDataBindType::FiledNameList:
						{
							TArray<FString> FilterValueArray;
							FString CleanFilterValue = DataBind.FilterValue.Replace(TEXT("；"), TEXT(";"));
							CleanFilterValue.ParseIntoArray(FilterValueArray, TEXT(";"), true);
							for (FString FilterValue : FilterValueArray)
							{
								FMQDataSourceTableDataRequestBodyColumnsData ColumnData;
								ColumnData.aggregator = 1;
								ColumnData.aliasType = TEXT("字段");

								FMQDataSourceColumnInfoItem* ColumnInfoItem = LocalFind::FindItem(ResponseColumns, FilterValue);
								if (ColumnInfoItem)
								{
									ColumnData.filedName = ColumnInfoItem->fieldName;
									ColumnData.dataType = FCString::Atoi(*ColumnInfoItem->dataType);
									//ColumnData.type = ColumnInfoItem->type;
									ColumnData.type = "DIMENSION";
									ColumnData.positionRange.start = -1;
									ColumnData.positionRange.end = -1;

									TableRequest.body.columns.Add(ColumnData);
								}
							}						
						}
						break;
					}// DataBind.BindType switch
				}// DataBinds for
			}
			else
			{
				// We will request all table data if there's NO data bind.
				FMQDataSourceTableDataRequestBodyColumnsData ColumnData;
				ColumnData.aggregator = 1;
				ColumnData.aliasType = TEXT("字段");
				ColumnData.type = "null";
				ColumnData.positionRange.start = 0;
				ColumnData.positionRange.end = ResponseColumns.Num();
				TableRequest.body.columns.Add(ColumnData);
			}
			
			//request.body.conditions
			for (FFilterRule FilterRule :FiterRules)
			{
				FMQDataSourceTableDataRequestBodyCondition ConditionData;
				ConditionData.relation = "AND";
				ConditionData.conditionType = (int)FilterRule.FilterType;
				if (FilterRule.FilterType == EFilterType::in)
				{
					TArray<FString> FilterValueArray;
					FString CleanFilterValue = FilterRule.FilterValue.Replace(TEXT("，"), TEXT(","));
					CleanFilterValue.ParseIntoArray(FilterValueArray, TEXT(","), true);
					for (FString FilterValue : FilterValueArray)
					{
						ConditionData.conditionValue.Add(FilterValue);
					}
				}
				else
				{
					ConditionData.conditionValue.Add(FilterRule.FilterValue);
				}

				FMQDataSourceColumnInfoItem* ColumnInfoItem = LocalFind::FindItem(ResponseColumns, FilterRule.ColumnName);
				if (ColumnInfoItem)
				{
					ConditionData.filedName = ColumnInfoItem->fieldName;
					ConditionData.dataType = FCString::Atoi(*ColumnInfoItem->dataType);
					//ConditionData.type = ColumnInfoItem->type;
					ConditionData.type = "DIMENSION";
				}
				TableRequest.body.conditions.Add(ConditionData);
			}
			////或
			//for (FFilterRule FilterRule : OrFiterRules)
			//{
			//	FMQDataSourceTableDataRequestBodyCondition ConditionData;
			//	ConditionData.relation = "OR";
			//	ConditionData.conditionType = (int)FilterRule.FilterType;
			//	if (FilterRule.FilterType == EFilterType::in)
			//	{
			//		TArray<FString> FilterValueArray;
			//		FString CleanFilterValue = FilterRule.FilterValue.Replace(TEXT("，"), TEXT(","));
			//		CleanFilterValue.ParseIntoArray(FilterValueArray, TEXT(","), true);
			//		for (FString FilterValue : FilterValueArray)
			//		{
			//			ConditionData.conditionValue.Add(FilterValue);
			//		}
			//	}
			//	else
			//	{
			//		ConditionData.conditionValue.Add(FilterRule.FilterValue);
			//	}

			//	FMQDataSourceColumnInfoItem* ColumnInfoItem = LocalFind::FindItem(ResponseColumns, FilterRule.ColumnName);
			//	if (ColumnInfoItem)
			//	{
			//		ConditionData.filedName = ColumnInfoItem->fieldName;
			//		ConditionData.dataType = FCString::Atoi(*ColumnInfoItem->dataType);
			//		//ConditionData.type = ColumnInfoItem->type;
			//		ConditionData.type = "DIMENSION";
			//	}
			//	TableRequest.body.conditions.Add(ConditionData);
			//}

			//request.body.orders
			for (FDataSort DataSort : Sorts)
			{
				FMQDataSourceTableDataRequestBodyOrder OrderData;
				OrderData.filedName = DataSort.ColumnName;
				OrderData.descending = DataSort.SortType == EDataSortType::Down;
				TableRequest.body.orders.Add(OrderData);
			}
			
			if (TableRequest.body.columns.Num() > 0)
			{
				FMqResponseCallback Callback;
				Callback.BindDynamic(this, &UMESComponetBase::OnDACTableDataResponse);
				//Callback.BindUFunction(this, "OnDACTableDataResponse");
				RabbitMqComponent->DataSourceTableDataRequest(TableRequest, Callback);
			}
			else
			{
				UpdateDataCallBack.Broadcast(TableData);
				DataUpdate();
			}
		}
	}

}

void UMESComponetBase::OnDACTableDataResponse(FString JsonString)
{
	FMQDataSourceTableDataResponse Response = URabbitMqProtocol::ParseMQDataSourceTableDataResponse(JsonString);
	if (FCString::Atoi64(*(Response.result.header.filterRuleId)) != GetFilterRuleId())
	//if (FCString::Atoi(*(Response.result.header.filterRuleId)) != GetFilterRuleId())
	{
		return;
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("filterRuleId pass, this: %s"), *GetFName().ToString());
	}

	AMESPlayerController* PlayerController = Cast<AMESPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PlayerController && PlayerController->IsValidLowLevelFast())
	{
		URabbitMqClient* RabbitMqComponent = PlayerController->RabbitMqComponent;
		if (RabbitMqComponent)
		{
			FMQDataSourceTableDataResponseBody ResponseBody = Response.result.body;

			// convert to FTableData
			TableData.Headers.Empty();
			TableData.RowDatas.Empty();

			//headers
			for (FMQDataSourceTableDataResponseBodyFieldAttribute FieldAttribute : ResponseBody.fieldAttribute)
			{
				FTableFiled tableFiled;
				tableFiled.FieldIndex = FieldAttribute.position;
				tableFiled.FieldName = FieldAttribute.filedName;
				tableFiled.HeaderType = "string";
				TableData.Headers.Add(tableFiled);
			}

			//row datas
			TableData.RowDatas = ResponseBody.rowData;
			if (TableData.RowDatas.Num() <= 0)
			{
				UE_LOG(LogTemp, Warning, TEXT("no Data "));
			}
			UpdateDataCallBack.Broadcast(TableData);
			DataUpdate();
		}
	}
}

#undef LOCTEXT_NAMESPACE