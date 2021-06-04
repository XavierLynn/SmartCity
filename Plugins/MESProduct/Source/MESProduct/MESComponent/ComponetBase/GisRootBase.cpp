// Fill out your copyright notice in the Description page of Project Settings.


#include "GisRootBase.h"

void AGisRootBase::RegisterGisPos()
{
	for (int k = 0; k < FixPos.Num(); k++)
	{
		FGisPoint* p = &(FixPos[k]);
		for (int i = 0; i < p->PosList.Num(); i++)
		{
			p->PosList[i].UnrealPos = p->Location;
			EGISType t = p->PosList[i].GisType;
			if (!GisDic.Contains(t))
			{
				TArray<FGISPos> a;
				GisDic.Add(t, a);
				GisDic[t].Add(p->PosList[i]);
				continue;
			}
			TArray<FGISPos>& list = GisDic[t];
			int curIndex = -1;
			//按从左到右，从上到下的顺序 进行 插入
			for (int j = 0; j < list.Num(); j++)
			{
				double Lon = p->PosList[i].Lon.GetValue();
				double ListLon = list[j].Lon.GetValue();
				if (Lon < ListLon)
				{
					curIndex = j;
					break;
				}

				if (Lon == ListLon)
				{
					double Lat = p->PosList[i].Lat.GetValue();
					double ListLat = list[j].Lat.GetValue();
					if (Lat == ListLat)
					{
						curIndex = -1;
						break;
					}
					if (Lat > ListLat)
					{
						curIndex = j;
						break;
					}
				}
				curIndex = j;
			}
			if (curIndex == -1)
			{
				break;
			}
			list.Insert(p->PosList[i], curIndex);
		}
	}
}

FVector AGisRootBase::GetPos(EGISType gtype, float x, float y, float z)
{
	return GetPosUseDouble(gtype, x, y, z);
}

void AGisRootBase::ClearGisDic()
{
	GisDic.Empty();
}

FString AGisRootBase::GetRefBoneName(int index)
{
	return Skeleton->GetReferenceSkeleton().GetBoneName(index).ToString();
}

TArray<FTransform> AGisRootBase::GetRefTransform()
{
	return Skeleton->GetReferenceSkeleton().GetRefBonePose();
}

void AGisRootBase::SetGisRoot()
{
	CurGisRoot = this;
}

void AGisRootBase::SetScale(float inscale)
{
	if(inscale == 0.f)
	{
		return;
	}
	Scale = inscale;
}

FVector AGisRootBase::GetPosUseDouble(EGISType gtype, double x, double y, double z)
{
	if (gtype == EGISType::MES)
	{
		return FVector((float)x, (float)y, (float)z);
	}
	if (!GisDic.Contains(gtype) || GisDic[gtype].Num() <= 1)
	{
		return FVector::ZeroVector;
	}
	TArray<FGISPos>& list = GisDic[gtype];
	for (int i = 0; i < list.Num(); i++)
	{
		double GisX = list[i].Lon.GetValue();
		double GisY = list[i].Lat.GetValue();
		if (x < GisX)
		{
			return CaculatPos(x, y, z, list, i);
		}
		if (x == GisX && y < GisY)
		{
			return CaculatPos(x, y, z, list, i);
		}
		if (x == GisX && y == GisY)
		{
			return list[i].UnrealPos;
		}
	}
	return CaculatPos(x, y, z, list, 0);
}

AGisRootBase* AGisRootBase::CurGisRoot = nullptr;

FVector AGisRootBase::CaculatPos(double X, double Y, double Z, TArray<FGISPos> list, int LefIndex)
{
	if (LefIndex == list.Num() - 1)
	{
		LefIndex = list.Num() - 2;
	}
	FGISPos g1 = list[LefIndex];
	FGISPos g2 = list[LefIndex + 1];
	double durX = g2.Lon.GetValue() - g1.Lon.GetValue();
	double durZ = g2.Lat.GetValue() - g1.Lat.GetValue();
	float durPosX = g2.UnrealPos.X - g1.UnrealPos.X;
	float durPosY = g2.UnrealPos.Y - g1.UnrealPos.Y;
	float x = (float)((X - g1.Lon.GetValue()) / durX * durPosX + g1.UnrealPos.X);
	float y = (float)((Y - g1.Lat.GetValue()) / durZ * durPosY + g1.UnrealPos.Y);
	return FVector(x, y, (float)Z);
}


void AGisRootBase::BeginPlay()
{
	Super::BeginPlay();
	SetGisRoot();
	RegisterGisPos();
}

void AGisRootBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	CurGisRoot = nullptr;

	Super::EndPlay(EndPlayReason);
}


#if WITH_EDITOR
void AGisRootBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (Skeleton == nullptr)
	{
		return;
	}
	const TArray<FTransform> CoordTrans = Skeleton->GetReferenceSkeleton().GetRefBonePose();
	for (int i = 0; i < CoordTrans.Num(); i++)
	{
		FString ParenName = Skeleton->GetReferenceSkeleton().GetBoneName(i).ToString();
		if (ParenName.Contains("|"))
		{
			if (FixPos.FindByPredicate([=](const FGisPoint& p) {return p.Name == ParenName; }) == nullptr)
			{
				FGisPoint p;	
				p.Name = ParenName;
				TArray<FString> OutStr;
				ParenName.ParseIntoArray(OutStr, TEXT("|"));
				if (OutStr.Num() < 2)
				{
					continue;
				}
				FGISPos gisPos;
				gisPos.GisType = EGISType::WGS84;
				FString FirstNum = OutStr[0].Replace(TEXT("_"), TEXT("."));
				gisPos.Lon.StringValue = FirstNum;
				FString SecondNum = OutStr[1].Replace(TEXT("_"), TEXT("."));
				gisPos.Lat.StringValue = SecondNum;
				UE_LOG(LogTemp, Warning, TEXT("%f"),Scale);
				p.Location = CoordTrans[i].GetLocation() * Scale;
				
				p.PosList.Add(gisPos);
				FixPos.Add(p);
			}
		}
	}
}
#endif