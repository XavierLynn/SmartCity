// Fill out your copyright notice in the Description page of Project Settings.


#include "PlateMapActor.h"
#include "MESProduct\MESComponent\ComponetBase\GisRootBase.h"
#include "EMGISManager.h"

// Sets default values
APlateMapActor::APlateMapActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PMC = CreateDefaultSubobject<UProceduralMeshComponent>("PMCMesh");
	PMC->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void APlateMapActor::BeginPlay()
{
	Super::BeginPlay();


	
	HttpResponseDelegate.BindUFunction(this, "GetResult");
	SiteHttpResponseDelegate.BindUFunction(this, "GetConditionData");

}

// Called every frame
void APlateMapActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APlateMapActor::GetRespoent()
{
	FString URL = "http://10.5.37.214:1202/siteSelectionCondition/findById/" + ActorID;
	UDigitalFrameNetWorkBlueprintLibrary::HttpRequest(EHttpVerb::Get, URL,TEXT(""), HttpResponseDelegate);

}

void APlateMapActor::GetSiteCondition()
{
	FString URL = "http://10.5.37.214:1202/siteSelectionCondition/queryAll?siteId=" + ActorID;
	UDigitalFrameNetWorkBlueprintLibrary::HttpRequest(EHttpVerb::Get, URL, TEXT(""), SiteHttpResponseDelegate);
}

void APlateMapActor::SetActorID(const FString& ID)
{
	ActorID = ID;
}

FString APlateMapActor::GetActorID()
{
	return ActorID;
}

void APlateMapActor::GetResult(const FString& Result)
{
	if (!Result.IsEmpty())
	{
		TSharedRef< TJsonReader<> > JsonReader = TJsonReaderFactory<>::Create(Result);
		TSharedPtr<FJsonObject> JsonMeg;
		if (FJsonSerializer::Deserialize(JsonReader, JsonMeg))
		{
			AnalysisHttpData(JsonMeg);
		}
	}
}

void APlateMapActor::AnalysisHttpData(TSharedPtr<FJsonObject> JsonMeg)
{
	if (JsonMeg->HasField(TEXT("data")))
	{
		TSharedPtr<FJsonObject> Data = JsonMeg->GetObjectField(TEXT("data"));
		if (Data.IsValid())
		{
			if (Data->HasField(TEXT("siteGeometry")))
			{
				FString SiteGeometry = Data->GetStringField(TEXT("siteGeometry"));
				TSharedRef< TJsonReader<> > SiteGeometryReader = TJsonReaderFactory<>::Create(SiteGeometry);
				TSharedPtr<FJsonObject> SiteGeometryMeg;
				if (FJsonSerializer::Deserialize(SiteGeometryReader, SiteGeometryMeg))
				{
					if (SiteGeometryMeg->HasField(TEXT("coordinates")))
					{
						TArray<TSharedPtr<FJsonValue>> Coordinates = SiteGeometryMeg->GetArrayField("coordinates");
						if (Coordinates.IsValidIndex(0))
						{
							if (Coordinates[0].IsValid())
							{
								
								TArray<TSharedPtr< FJsonValue >> CoordinatePoints = Coordinates[0]->AsArray();
								if (CoordinatePoints[0].IsValid())
								{
									TArray<TSharedPtr< FJsonValue >> CurCoordinatePoints = CoordinatePoints[0]->AsArray();
									if (CurCoordinatePoints.IsValidIndex(0))
									{
										for (auto CoordinatePoint : CurCoordinatePoints)
										{
											TArray<TSharedPtr< FJsonValue >> CoordinatePoint2D = CoordinatePoint->AsArray();
											FVector CurCoordinate;
											CurCoordinate.X = CoordinatePoint2D[0]->AsNumber();
											CurCoordinate.Y = CoordinatePoint2D[1]->AsNumber();
											CurCoordinate.Z = 0;
											PlateCoordinates.Add(CurCoordinate);

										}
									}
								}
								

							}
					
						}
					}
				}

			}
		}


	}

	GenerateMesh();
}

void APlateMapActor::AnalysisSiteHttpData(TSharedPtr<FJsonObject> JsonMeg)
{
	TMap<FString, FString> ReturnSiteValues;
	FSiteData CurSiteData;
	if (JsonMeg->HasField(TEXT("data")))
	{
		TArray<TSharedPtr<FJsonValue>> Data = JsonMeg->GetArrayField("data");
		if (Data.IsValidIndex(0)&&Data[0].IsValid())
		{
			TSharedPtr<FJsonObject> CurJson =Data[0]->AsObject();
			if (CurJson->HasField(TEXT("id")))
			{
				FString CurId = CurJson->GetStringField("id");
				ReturnSiteValues.Add("id", CurId);
			}
			if (CurJson->HasField(TEXT("code")))
			{
				FString CurCode = CurJson->GetStringField("code");
				ReturnSiteValues.Add("code", CurCode);
			}
			if (CurJson->HasField(TEXT("name")))
			{
				FString CurName = CurJson->GetStringField("name");
				ReturnSiteValues.Add("name", CurName);
			}
			if (CurJson->HasField(TEXT("company")))
			{
				FString CurCompany = CurJson->GetStringField("company");
				ReturnSiteValues.Add("company", CurCompany);
			}
			if (CurJson->HasField(TEXT("type")))
			{
				FString CurType = CurJson->GetStringField("type");
				ReturnSiteValues.Add("type", CurType);
			}
			if (CurJson->HasField(TEXT("statusName")))
			{
				FString CurStatusName = CurJson->GetStringField("statusName");
				ReturnSiteValues.Add("statusName", CurStatusName);
			}
		}
	}
	CurSiteData.RetrunJsonValue = ReturnSiteValues;\
	DelegateReturnSiteValue.Broadcast(CurSiteData);
}

void APlateMapActor::GetConditionData(const FString& Result)
{
	if (!Result.IsEmpty())
	{
		TSharedRef< TJsonReader<> > JsonReader = TJsonReaderFactory<>::Create(Result);
		TSharedPtr<FJsonObject> JsonMeg;
		if (FJsonSerializer::Deserialize(JsonReader, JsonMeg))
		{
			AnalysisSiteHttpData(JsonMeg);
		}
	}
}

void APlateMapActor::GenerateMesh()
{
	GISManager = new EMGISManager();

	ClearMeshData();

	TArray<FVector> Normals;
	TArray<FVector2D> UV;
	TArray<FColor> VertexColors;
	TArray<FProcMeshTangent> Tangents;
	if (PlateCoordinates.IsValidIndex(0))
	{
		for (auto CurVertor : PlateCoordinates)
		{
			FVector2D WGSGIS = GISManager->GCJTransformToWGS(CurVertor.X, CurVertor.Y);
			/*	FVector2D UELocation = GISManager->LoadFromGIS(WGSGIS.X, WGSGIS.Y, GISCenter.X, GISCenter.Y);
				FVector Point = FVector(UELocation.X, UELocation.Y, 0);*/
			if (AGisRootBase::CurGisRoot == nullptr)
			{
				break;
			}
			FVector Pos = AGisRootBase::CurGisRoot->GetPosUseDouble(EGISType::WGS84, WGSGIS.X, WGSGIS.Y, 0);
			if (Pos.X > 100000000 || Pos.X < -100000000 || Pos.Y > 100000000 || Pos.Y < -100000000 || Pos.Z > 100000000 || Pos.Z < -100000000)
			{
				continue;
			}
			Pos = FVector(Pos.X + 20410, Pos.Y + 25830, 0);
			Verties.Add(Pos);
		}
		Triangles.Add(0);
		Triangles.Add(1);
		Triangles.Add(2);
		for (int i = 2; i < PlateCoordinates.Num() - 2; i++)
		{
			Triangles.Add(i);
			Triangles.Add(i + 1);
			Triangles.Add(0);
		}

	}

	PMC->CreateMeshSection(0, Verties, Triangles, Normals, UV, VertexColors, Tangents, true);

	PMC->SetMaterial(0, MeshMaterial);
}

void APlateMapActor::ClearMeshData()
{
	Verties.Empty();
	Triangles.Empty();
}
