// Fill out your copyright notice in the Description page of Project Settings.


#include "MESPointComponetBase.h"
#include "MESProduct\MESComponent\ComponetBase\GisRootBase.h"
#include "Engine\Classes\Kismet\KismetSystemLibrary.h"
#include "Engine\Classes\Kismet\GameplayStatics.h"

void AMESPointComponetBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	ClearPoint();
}

void AMESPointComponetBase::BeginPlay()
{
	Super::BeginPlay();
}

void AMESPointComponetBase::OnReceiveData()
{
	Super::OnReceiveData();
	ClearPoint();
	FString Name =  UKismetSystemLibrary::GetDisplayName(this);
	if (!PointDic.Contains(Name))
	{
		PointDic.Add(Name,this);
	}
	//根据坐标数据进行创建的打点
	if (DataSourceType == EDataSourceType::NONE)
	{
		AActor* result = GetWorld()->SpawnActor<AActor>(PointActor, CommonData.Pos, CommonData.Rotation.Rotator());
		if (result == nullptr)
		{
			return;
		}
		result->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
		APointActorBase* pb = Cast<APointActorBase>(result);
		pb->SetActorHiddenInGame(!CommonData.Visible);
		pb->PointMain = this;
		Points.Add(pb);
		return;
	}
	this->SetActorLocation(FVector::ZeroVector);
	this->SetActorRotation(FRotator::ZeroRotator);
	this->SetActorScale3D(FVector::OneVector);
	FMESCustomConfig* LocationConfig = CustomConfig.Find(TEXT("坐标系"));
	int enumIndex = -1;
	if (LocationConfig != nullptr)
	{
		UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EGISType"), true);
		enumIndex = EnumPtr->GetIndexByNameString(LocationConfig->FiledValue);
	}
	EGISType gisType = EGISType::MES;
	if (enumIndex != -1)
	{
		gisType = (EGISType)enumIndex;
	}
	TArray<FString> LonStr = GetDataColoumn(TEXT("经度"));
	TArray<FString> LatStr = GetDataColoumn(TEXT("纬度"));
	TArray<FString> StrHeigh = GetDataColoumn(TEXT("高度"));
	for (int i = 0; i < LonStr.Num(); i++)
	{
		double Lon = FCString::Atod(*LonStr[i]);
		double Lat = 0;
		if (i < LatStr.Num())
		{
			Lat = FCString::Atod(*LatStr[i]);
		}
	    float height = 0;
		if (i < StrHeigh.Num())
		{
			height = FCString::Atod(*StrHeigh[i]);
		}
		//开始创打点
		if (AGisRootBase::CurGisRoot == nullptr)
		{
			break;
		}
		FVector pos = AGisRootBase::CurGisRoot->GetPosUseDouble(gisType, Lon, Lat, height);
		if (pos.X > 100000000 || pos.X < -100000000 || pos.Y > 100000000 || pos.Y < -100000000 || pos.Z > 100000000 || pos.Z < -100000000)
		{
			continue;//非法坐标
		}
		/*AActor* result = GetWorld()->SpawnActor<AActor>(PointActor,pos, FRotator::ZeroRotator);*/
		FTransform trans;
		trans.SetLocation(pos);
		APointActorBase* pb = Cast<APointActorBase>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, PointActor, trans));
		if (pb == nullptr)
		{
			continue;
		}
		pb->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
		pb->SetActorHiddenInGame(!CommonData.Visible);
		pb->PointMain = this;
		pb->PointData = TableData.RowDatas[i].ColumnDatas;
		UGameplayStatics::FinishSpawningActor(pb, trans);
		pb->OnDataInit();
		Points.Add(pb);
	}
	UpdateMainPointTrans();
}

TMap<FString, AMESPointComponetBase*> AMESPointComponetBase::PointDic;

AMESPointComponetBase::AMESPointComponetBase()
{
	FString Lon = TEXT("经度");
	DataBinds.Add(Lon, FDataBind());
	FString Lat = TEXT("纬度");
	DataBinds.Add(Lat, FDataBind());
	FString Height = TEXT("高度");
	DataBinds.Add(Height, FDataBind());
	FString LocationType = TEXT("坐标系");
	FMESCustomConfig locationConfig;
	locationConfig.FiledType = "string";
	CustomConfig.Add(LocationType, FMESCustomConfig());
}

void AMESPointComponetBase::SetFinishForPoint()
{
	GetComponetData();
}

void AMESPointComponetBase::SetCommonDataInfo(FActorBaseData data)
{
	int UUID = CommonData.UUID;
	CommonData = data;
	CommonData.UUID = UUID;
}

void AMESPointComponetBase::ShowPoints(FString PointCompnetName)
{
	if (!PointDic.Contains(PointCompnetName))
	{
		return;
	}
	AMESPointComponetBase* point = PointDic[PointCompnetName];
	for (int i = 0; i < point->Points.Num(); i++)
	{
		point->Points[i]->SetActorHiddenInGame(false);
	}
}

void AMESPointComponetBase::HidePoints(FString PointCompnetName)
{
	if (!PointDic.Contains(PointCompnetName))
	{
		return;
	}
	AMESPointComponetBase* point = PointDic[PointCompnetName];
	for (int i = 0; i < point->Points.Num(); i++)
	{
		point->Points[i]->SetActorHiddenInGame(true);;
	}
}

void AMESPointComponetBase::SetMainTrans(FTransform trans)
{
	if (DataSourceType == EDataSourceType::NONE)
	{
		CommonData.Pos = trans.GetLocation();
		CommonData.Rotation = trans.GetRotation();
		CommonData.Size = trans.GetScale3D();
	}
}

APointActorBase* AMESPointComponetBase::GetFirstPoint()
{
	if (Points.Num() == 0)
	{
		return nullptr;
	}
	return Points[0];
}

APointActorBase* AMESPointComponetBase::GetNextPoint(APointActorBase* curPoint)
{
	for (int i = 0; i < Points.Num(); i++)
	{
		if (Points[i] == curPoint && i != Points.Num() - 1)
		{
			return Points[i + 1];
		}
	}
	return GetFirstPoint();
}

void AMESPointComponetBase::UpdateMainPointTrans()
{
	//根据坐标数据进行创建的打点
	if (DataSourceType == EDataSourceType::NONE)
	{
		Points[0]->SetActorLocation(CommonData.Pos);
		Points[0]->SetActorRotation(CommonData.Rotation);
		Points[0]->SetActorScale3D(CommonData.Size);
		return;
	}
	if (Points.Num() == 0)
	{
		this->SetActorLocation(CommonData.Pos);
		this->SetActorRotation(CommonData.Rotation);
		this->SetActorScale3D(CommonData.Size);
	}
}

void AMESPointComponetBase::ClearPoint()
{
	for (int i = 0; i < Points.Num(); i++)
	{
		AActor* p = Points[i];
		p->Destroy();
	}
	Points.Empty();
	PointDic.Remove(UKismetSystemLibrary::GetDisplayName(this));
}
