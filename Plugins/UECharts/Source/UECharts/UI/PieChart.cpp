// Fill out your copyright notice in the Description page of Project Settings.


#include "PieChart.h"

UPieChart::UPieChart()
{
	PieInternal = FMath::Clamp(PieInternal, 0.f, 1.f);
}

float UPieChart::PieAnimation(const FVector2D InMouseLocationOnViewpot)
{
	IsHover = true;
	FVector2D RelativeLocation = InMouseLocationOnViewpot - CircleCenter;
	FVector2D ZeroVector = FVector2D(FMath::Abs(RelativeLocation.X), 0);
	float t = ((RelativeLocation.X) * (ZeroVector.X)+(RelativeLocation.Y) * (ZeroVector.Y)) / (FMath::Sqrt(FMath::Pow(RelativeLocation.X, 2) + FMath::Pow(RelativeLocation.Y, 2)) * FMath::Sqrt(FMath::Pow(ZeroVector.X, 2) + FMath::Pow(ZeroVector.Y, 2)));
	float MouseCapsToCirCenterDistance = FVector2D::Distance(InMouseLocationOnViewpot, CircleCenter);
	if ((Width / 2 - CircleWidth) <=MouseCapsToCirCenterDistance&& MouseCapsToCirCenterDistance <= (Width/2))
	{
		AngleValue = FMath::RadiansToDegrees(acos(t))-InternalAngle;
	
		if (RelativeLocation.Y > 0)
		{
			AngleValue = 360- InternalAngle - InternalAngle - AngleValue;
			
		}
		if (AngleValue < 0)
		{
			AngleValue = 360  - FMath::Abs(AngleValue);
		}

		for (int i = 0;i<PieIndex_Angle.Num();i++)
		{
			if (int(PieIndex_Angle.FindRef(i)/2) == int(AngleValue))
			{
				AngleCenters.AddUnique(InMouseLocationOnViewpot);
			}
			if (i==0)
			{
				if (AngleValue < PieIndex_Angle.FindRef(0) && AngleValue >= 0)
				{
					for (auto CurPieSerie : PieSeries)
					{
						if (CurPieSerie.index == i)
						{
							CurIndex = i;
							OnPieHover.Broadcast(CurPieSerie);
						}
					}
					
				}
			}
			else {
				if (AngleValue < PieIndex_Angle.FindRef(i) && AngleValue >= PieIndex_Angle.FindRef(i-1))
				{
					for (auto CurPieSerie : PieSeries)
					{
						if (CurPieSerie.index == i)
						{
							CurIndex = i;
							OnPieHover.Broadcast(CurPieSerie);
						}
					}

				}
			}
			
		}
		
		return AngleValue;
	}
	CurIndex = -1;
	AngleValue = -1;
	UnPieHover.Broadcast();
	return AngleValue;

}

void UPieChart::HoverPieItem(const int CurHoverIndex)
{

	CurIndex = CurHoverIndex;
	for (auto PieSerie : PieSeries)
	{
		if (PieSerie.index == CurHoverIndex)
		{
			OnPieHover.Broadcast(PieSerie);
		}
		
	}
	
}

TArray<FVector2D> UPieChart::GetAngleCenter()
{
	return AngleCenters;
}

TMap<int, FVector4> UPieChart::GetAllAngleCenterPoints(EPieRiders InPieRiders,float OtherRiders)
{

	TMap<int, FVector4> AllAngleCenterPoints;
	AllAngleCenterPoints.Empty();
	TArray<float> CurAngles;
	for (int i = 0; i < PieIndex_Angle.Num(); i++)
	{
		if (PieIndex_Angle.Find(i)==0)
			continue;
		float PieCenterDegrees;
		float MaxRadis = 0;
		CircleWidth = CircleWidth > (Width / 2) ? Width / 2 : CircleWidth;
		switch (InPieRiders)
		{
		case EPieRiders::Outer:
			MaxRadis =  Width / 2;
			break;

		case EPieRiders::Center:
			MaxRadis =  Width / 2 - CircleWidth / 2;
			break;
		case EPieRiders::Inner:
			MaxRadis =  Width / 2 - CircleWidth;
			break;
		case EPieRiders::Other:
			MaxRadis =  Width / 2+ OtherRiders;
			break;
		default:
			break;
		}
		int X;
		int Y;
		FVector4 PieCenterPoint;
		if (i==0)
		{
			PieCenterDegrees = (PieIndex_Angle.FindRef(0) - (CurPieInter * 360 / SumPieData)) / 2 + StartAngle;
		}
		else
		{
			PieCenterDegrees = (PieIndex_Angle.FindRef(i) - PieIndex_Angle.FindRef(i - 1) - (CurPieInter * 360 / SumPieData)) / 2 + PieIndex_Angle.FindRef(i - 1) + StartAngle;
		}
		CurAngles.Add(PieCenterDegrees);
		X = (0 <= int(PieCenterDegrees) % 360 && int(PieCenterDegrees) % 360 <=90 )|| (270 <= int(PieCenterDegrees) % 360 && int(PieCenterDegrees) % 360 < 360 )? 0 : 1;
		Y = 0 <= int(PieCenterDegrees) % 360 && int(PieCenterDegrees) % 360 < 180 ? 1 : 0;
	
		PieCenterPoint = FVector4(CircleCenter.X+MaxRadis * FMath::Cos(-FMath::DegreesToRadians(PieCenterDegrees)), CircleCenter.Y+MaxRadis * FMath::Sin(-FMath::DegreesToRadians(PieCenterDegrees)), X, Y);
		AllAngleCenterPoints.Add(i,PieCenterPoint);
	}
	return AllAngleCenterPoints;
}

void UPieChart::CalculateLabelPositionY(TMap<int, float>& LeftLabelPosition, TMap<int, float>& RightLabelPosition,const TMap<int, FVector4> CurPieCenterPoints, int32 InLabelFontSize )
{
	for (auto Elem:CurPieCenterPoints)
	{
		if (Elem.Value.Z > 0)
			LeftLabelPosition.Add(Elem.Key, Elem.Value.Y);
		else
			RightLabelPosition.Add(Elem.Key, Elem.Value.Y);
	}
	LeftLabelPosition.ValueSort([](float A, float B)
		{
			return  A < B;
		});
	RightLabelPosition.ValueSort([](float A, float B)
		{
			return  A < B;
		});
	TMap<int, float> CurLabelPosition;
	float CurYValue = -10000000;
	for (auto LeftElem : LeftLabelPosition)
	{
		if (CurYValue == -10000000|| LeftElem.Value - CurYValue > InLabelFontSize)
		{
			CurYValue = LeftElem.Value;
			CurLabelPosition.Add(LeftElem.Key, CurYValue);
		}
		else
		{
			CurLabelPosition.Add(LeftElem.Key, CurYValue + InLabelFontSize);
			CurYValue = CurYValue + InLabelFontSize;
		}
	}
	LeftLabelPosition = CurLabelPosition;
	CurLabelPosition.Empty();
	CurYValue = -10000000;
	for (auto RightElem : RightLabelPosition)
	{
		if (CurYValue == -10000000 || RightElem.Value - CurYValue > InLabelFontSize)
		{
			CurYValue = RightElem.Value;
			CurLabelPosition.Add(RightElem.Key, CurYValue);
		}
		else
		{
			CurLabelPosition.Add(RightElem.Key, CurYValue + InLabelFontSize);

			CurYValue = CurYValue + InLabelFontSize;
		}
	}
	RightLabelPosition = CurLabelPosition;
}

void UPieChart::CalculateLabelPositionVector(TMap<int, FVector2D>& LeftLabelPosition, TMap<int, FVector2D>& RightLabelPosition, const TMap<int, FVector4> CurPieCenterPoints, const int32 InLabelFontSize,const int32 LabelLineRadisOuterOffset)
{
	for (auto Elem : CurPieCenterPoints)
	{
		if (Elem.Value.Z > 0)
			LeftLabelPosition.Add(Elem.Key, FVector2D(Elem.Value.X,Elem.Value.Y));
		else
			RightLabelPosition.Add(Elem.Key, FVector2D(Elem.Value.X, Elem.Value.Y));
	}
	LeftLabelPosition.ValueSort([](FVector2D A, FVector2D B)
		{
			return  A.Y < B.Y;
		});
	RightLabelPosition.ValueSort([](FVector2D A, FVector2D B)
		{
			return A.Y < B.Y;
		});
	TMap<int, FVector2D> CurLabelPosition;
	float CurYValue = -10000000;
	for (auto LeftElem : LeftLabelPosition)
	{
		if (CurYValue == -10000000 || LeftElem.Value.Y - CurYValue > InLabelFontSize)
			CurYValue = LeftElem.Value.Y;
		else
			CurYValue = CurYValue + InLabelFontSize;
		float CurDistance = FMath::Sqrt(FMath::Pow((LeftElem.Value.X - CircleCenter.X), 2) + FMath::Pow((CurYValue - CircleCenter.Y), 2));
		bool LabelSign = (LeftElem.Value.X - CircleCenter.X > 0) ? true : false;
		float CurXValue = LeftElem.Value.X;
		if (CurDistance <= Width / 2 + LabelLineRadisOuterOffset)
		{
			CurXValue = (FMath::Sqrt(FMath::Pow((Width / 2), 2) - FMath::Pow((CurYValue - CircleCenter.Y), 2))) * (LabelSign ? 1 : -1) + CircleCenter.X - LabelLineRadisOuterOffset;
		}
		CurLabelPosition.Add(LeftElem.Key, FVector2D(CurXValue, CurYValue));
	}
	LeftLabelPosition = CurLabelPosition;
	CurLabelPosition.Empty();
	CurYValue = -10000000;

	for (auto RightElem : RightLabelPosition)
	{
		if (CurYValue == -10000000 || RightElem.Value.Y - CurYValue > InLabelFontSize)
			CurYValue = RightElem.Value.Y;
		else
			CurYValue = CurYValue + InLabelFontSize;
		float CurDistance = FMath::Sqrt(FMath::Pow((RightElem.Value.X - CircleCenter.X), 2) + FMath::Pow((CurYValue - CircleCenter.Y), 2));
		bool LabelSign = (RightElem.Value.X - CircleCenter.X > 0) ? true : false;
		float CurXValue = RightElem.Value.X;
		if (CurDistance <= Width / 2 + LabelLineRadisOuterOffset)
		{
			CurXValue = (FMath::Sqrt(FMath::Pow((Width / 2), 2) - FMath::Pow((CurYValue - CircleCenter.Y), 2))) * (LabelSign ? 1 : -1) + CircleCenter.X + LabelLineRadisOuterOffset;
		}
		CurLabelPosition.Add(RightElem.Key, FVector2D(CurXValue, CurYValue));
	}
	RightLabelPosition = CurLabelPosition;
}

void UPieChart::CalculateLabelOffsetVector(const float LabelOffset, float& RightLabelOffset, float& LeftLabelOffset)
{

	RightLabelOffset = CircleCenter.X + Width / 2 + LabelOffset;
	LeftLabelOffset = CircleCenter.X - Width / 2 - LabelOffset;
}

void UPieChart::ResetAngleValue()
{
	CurIndex = -1;
	AngleValue = -1.f;
	UnPieHover.Broadcast();
}

//TArray<FVector2D> UPieChart::FindPieElemCenterPoint()
//{
//	TArray<FVector2D> PieElemCenterPoint;
//	TMap<int, FVector4> AllAngleEdgePoint = GetAllAngleCenterPoints();
//	for (auto& Elem : AllAngleEdgePoint)
//	{
//		FVector2D CurCenterPoint = FVector2D(Elem.Value.X - CircleCenter.X,Elem.Value.Y-CircleCenter.Y);
//
//	}
//
//	return PieElemCenterPoint;
//}

void UPieChart::OnPopulateMesh(const FGeometry& AllottedGeometry, TArray<FSlateVertex>& vertex, TArray<SlateIndex>& index)
{

	PieInternal = FMath::Clamp(PieInternal, 0.f, 1.f);
	InternalAngle = (int)StartAngle % 360;
	vertex.Empty();
	index.Empty();
	float outer = (AllottedGeometry.Size.X < AllottedGeometry.Size.Y ? AllottedGeometry.Size.X : AllottedGeometry.Size.Y) * 0.5;
	
	FirstX = -outer;
	FirstY = outer;
	Width = outer*2;
	Height = Width;
	CircleCenter = FVector2D(AllottedGeometry.Size.X / 2, AllottedGeometry.Size.Y / 2);
	float inner = outer - CircleWidth;
	if(inner < 0.f)
	{
		inner = 0.f;
	}
	float Start =StartAngle + RenderTransform.Angle;
	float end = EndAngle + RenderTransform.Angle;
	if (Start > end)
	{
		Start = end;
		end = StartAngle + RenderTransform.Angle;
	}
	float OutRadis = outer;
	DataToRadian(PieSeries);
	for (int i = 0;i< PieSeries.Num();i++)
	{
		float* CurAngle = PieIndex_Angle.Find(PieSeries[i].index);
		if (PieSeries[i].Value == 0)
			continue;
		if (i == 0)
		{
			OutRadis = (0 <= AngleValue && AngleValue < *CurAngle - (CurPieInter * 360 / SumPieData) && AngleValue != -1)||CurIndex==i ? outer + OutRadisAdd : outer;
			GradiantColors = PieSeries[i].PieGradiantColors;
			DrawCircle(OutRadis, inner, InternalAngle, InternalAngle + *CurAngle - (CurPieInter * 360 / SumPieData), 0, 0, false, false);
		}
		else
		{
			float *PerAngle = PieIndex_Angle.Find(PieSeries[i-1].index);
			OutRadis = (*PerAngle <= AngleValue && AngleValue < *CurAngle - (CurPieInter * 360 / SumPieData) && AngleValue != -1)||CurIndex == i ? outer + OutRadisAdd : outer;
			GradiantColors = PieSeries[i].PieGradiantColors;
			DrawCircle(OutRadis, inner, *PerAngle + InternalAngle, *CurAngle - (CurPieInter * 360 / SumPieData) + InternalAngle, 0, 0, false, false);
		}
	}
	
	return;
}

void UPieChart::DrawCircle(float outer, float inner, float start, float end, float offsetX, float offsetY, bool OuterAnti,bool innerAnti)
{
	float x = outer * FMath::Sin(0);
	float y = outer * FMath::Cos(0);
	FVector2D prevOuter(x,y);
	x = inner * FMath::Cos(0);
	y = inner * FMath::Sin(0);
	FVector2D prevInner(x, y);
	FVector2D pos[4];
	FSlateVertex uiVertices[4];
	if (end - start < 0 || end - start > 360)
	{
		start = 0;
		end = 360;
	}
	else
	{
		float dur = end - start;
		int result = (int)start % 360;
		start = result + FMath::Fractional(start);
		end = start + dur;
	}
	bool Quit = false;
	for (float i = start; i <= end + 1 && Quit == false; i++)
	{
		if (i > end)
		{
			Quit = true;
			i = end;
		}

		float rad = -FMath::DegreesToRadians(i);
		float c = FMath::Cos(rad);
		float s = FMath::Sin(rad);

		pos[0] = prevOuter;
		pos[1] = FVector2D(outer * c - offsetX, outer * s - offsetY);
		pos[2] = FVector2D(inner * c - offsetX, inner * s - offsetY);
		pos[3] = prevInner;
		prevOuter = pos[1];
		prevInner = pos[2];
		if (i == start) continue;
		for (int j = 0; j < 4; j++)
		{
			uiVertices[j].Position = pos[j];
			if (GradiantStyle == EGradiantMethod::ZeroToFull)
			{
				uiVertices[j].Color = GetColorByAngle(start, end, i);
			}
			else if (GradiantStyle == EGradiantMethod::InToOut)
			{
				if (OuterAnti)
				{
					uiVertices[j].Color = GradiantColors[GradiantColors.Num() - 1];
				}
				else if (innerAnti)
				{
					uiVertices[j].Color = GradiantColors[0];
				}
				else
				{
					uiVertices[j].Color = GetColorByRadius(outer, inner, (j == 0 || j == 1) ? outer : inner);	
				}
			}
			else
			{
				uiVertices[j].Color = GetColorByPos(pos[j].X, pos[j].Y);
			}
			
			if(OuterAnti && (j == 0 || j == 1))
			{
				uiVertices[j].Color.A = 0;
			}
			if(innerAnti && (j == 2 || j == 3))
			{
				uiVertices[j].Color.A = 0;
			}
		}
		AddUIVertexQuad(uiVertices);
	}
}

void UPieChart::DataToRadian(TArray<FPieSeries>& InPieData)
{
	int32 PieItemValueIndex = 0;
	float CurSumPieData = 0;
	float CurAngle = 0;
	float AngleCenter = 0;
	PieIndex_Angle.Empty();
	for (int i = 0; i < InPieData.Num(); i++)
	{
		//float CurPieInternal = InPieData[i].Value <= 0 ? 0 : PieInternal;
		//CurSumPieData += (InPieData[i].Value + CurPieInternal);
		if (InPieData[i].Value>0)
		{
			PieItemValueIndex++;
		}
		CurSumPieData += InPieData[i].Value;
	}

	CurPieInter = PieItemValueIndex <= 1 ? 0 : PieInternal * CurSumPieData;

	CurSumPieData = 0;
	for (int i = 0; i < InPieData.Num(); i++)
	{
		float CurPieInternal = InPieData[i].Value <= 0 ? 0 : CurPieInter;
		CurSumPieData += (InPieData[i].Value + CurPieInternal);
	}
	SumPieData = CurSumPieData;
	for (int i = 0; i < InPieData.Num(); i++)
	{
		float CurPieInternal = InPieData[i].Value <= 0?0: CurPieInter;
		CurAngle += (InPieData[i].Value + CurPieInternal) * 360 / SumPieData;
		PieIndex_Angle.Add(InPieData[i].index, CurAngle);
	}
}

//void UPieChart::DrawLabelLine(const TMap<int, FVector4> CurAngleCenterPoint, float InFirstLableLengh, float InScendLabelLengh, float InLabelWith)
//{
//	if (CurAngleCenterPoint.Num() <= 0)
//	{
//		return;
//	}
//	FSlateVertex uiVertices[4];
//	for (auto& Elem : CurAngleCenterPoint)
//	{
//		FVector2D pos[4];
//		float OffsetX = Elem.Value.Z ? 1 : -1;
//		float OffsetY = Elem.Value.W ? 1 : -1;
//		pos[0] = FVector2D(Elem.Value.X , Elem.Value.Y );
//		pos[1] = FVector2D(Elem.Value.X+ OffsetX*InFirstLableLengh, Elem.Value.Y+ OffsetY*InFirstLableLengh);
//		pos[2] = FVector2D(Elem.Value.X + OffsetX * LabelWith / 4+ OffsetX*InFirstLableLengh, Elem.Value.Y + OffsetY * LabelWith / 4+ OffsetY*InFirstLableLengh);
//		pos[3] = FVector2D(Elem.Value.X + OffsetX * LabelWith, Elem.Value.Y + OffsetY * LabelWith);
//	/*	pos[0] = FVector2D(Elem.Value.X, Elem.Value.Y);
//		pos[1] = FVector2D(200, 200);
//		pos[2] = FVector2D(400, 100);
//		pos[3] = FVector2D(Elem.Value.X, Elem.Value.Y);*/
//		for (int j = 0; j < 4; j++)
//		{
//			uiVertices[j].Position = pos[j];
//			uiVertices[j].Color = FColor::Red;
//			uiVertices[j].Color.A = 255;
//		}
//		int currentVertCount = VertexData.Num();
//		for (int index = 0; index < 4; ++index)
//		{
//			VertexData.Add(uiVertices[index]);
//		}
//		IndexData.Add(currentVertCount);
//		IndexData.Add(currentVertCount + 1);
//		IndexData.Add(currentVertCount + 2);
//
//		IndexData.Add(currentVertCount + 2);
//		IndexData.Add(currentVertCount + 3);
//		IndexData.Add(currentVertCount);
//	}
//}
