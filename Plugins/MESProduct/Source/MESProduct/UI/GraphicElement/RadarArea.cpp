// Fill out your copyright notice in the Description page of Project Settings.


#include "RadarArea.h"


void URadarArea::OnPopulateMesh(const FGeometry& AllottedGeometry, TArray<FSlateVertex>& vertex, TArray<SlateIndex>& index)
{
	vertex.Empty();
	index.Empty();
	outerRadius = (AllottedGeometry.Size.X < AllottedGeometry.Size.Y ? AllottedGeometry.Size.X : AllottedGeometry.Size.Y) * 0.5;
	//开始计算值得合理范围
	if (Data.Num() < 3)return;
	Points.Empty();
	float step = 360.f / Data.Num();
	for (int i = -1; i < Data.Num(); i++)
	{
		float d = 0;
		FSlateVertex s;
		if(i != -1)
		{
			Data[i] = FMath::Clamp(Data[i], 0.f, 1.f);
			d = Data[i];
			s.Position =  GetPoint(outerRadius*Data[i], i*step + 90);
		}
		else
		{
			s.Position = FVector2D::ZeroVector;
		}
		
		if (GradiantMethod == EGradiantType::ZeroToFull)
		{
			s.Color = GetColorByAngle(0, 360, i * step);
		}
		else if (GradiantMethod == EGradiantType::InToOut)
		{
			s.Color = GetColorByRadius(1, 0,d);
		}
		else
		{
			s.Color = GetColorByPos(s.Position.X, s.Position.Y);
		}
		Points.Add(s);
		vertex.Add(s);
	}
	//return;

	for (int i = 1; i < Points.Num(); i++)
	{
		index.Add(0);
		index.Add(i);
		if (i == Points.Num() - 1)
		{
			index.Add(1);
		}
		else
		{
			index.Add(i + 1);
		}
	}

	FSlateVertex FirstPos1;
	FSlateVertex FirstPos2;
	FSlateVertex Last1;
	FSlateVertex Last2;
	for (int i = 1; i < Points.Num(); i++)
	{
		FSlateVertex LinePoints[4];
		LinePoints[0].Color = LineColor;
		LinePoints[1].Color = LineColor;
		LinePoints[2].Color = LineColor;
		LinePoints[3].Color = LineColor;
		if (i == Points.Num() - 1)
		{
			GetRectPoint(LinePoints, Points[i].Position, Points[1].Position, LineWidth);
		}
		else
		{
			GetRectPoint(LinePoints, Points[i].Position, Points[i + 1].Position, LineWidth);
		}
		AddUIVertexQuad(LinePoints);
		if (i == 1)
		{
			FirstPos1 = LinePoints[0];
			FirstPos2 = LinePoints[1];
			Last1 = LinePoints[2];
			Last2 = LinePoints[3];
		}
		else
		{
			int count = VertexData.Num();
			Points[i].Color = LineColor;
			VertexData.Add(Last2);
			VertexData.Add(Points[i]);
			VertexData.Add(LinePoints[0]);
			VertexData.Add(Last1);
			VertexData.Add(Points[i]);
			VertexData.Add(LinePoints[1]);
			index.Add(count);
			index.Add(count + 1);
			index.Add(count + 2);
			index.Add(count + 3);
			index.Add(count + 4);
			index.Add(count + 5);
			Last1 = LinePoints[2];
			Last2 = LinePoints[3];
		}
		if(i == Points.Num() -1)
		{
			int count = VertexData.Num();
			Points[1].Color = LineColor;
			VertexData.Add(Last2);
			VertexData.Add(Points[1]);
			VertexData.Add(FirstPos1);
			VertexData.Add(Last1);
			VertexData.Add(Points[1]);
			VertexData.Add(FirstPos2);
			index.Add(count);
			index.Add(count + 1);
			index.Add(count + 2);
			index.Add(count + 3);
			index.Add(count + 4);
			index.Add(count + 5);
		}
	}

}

FVector2D URadarArea::GetPoint(float radius, float angle)
{
	float rad = -FMath::DegreesToRadians(angle);
	float c = FMath::Cos(rad);
	float s = FMath::Sin(rad);
	return FVector2D(radius * c, radius * s);
}
