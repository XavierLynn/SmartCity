// Fill out your copyright notice in the Description page of Project Settings.


#include "LineChart.h"
#include "Blueprint/WidgetLayoutlibrary.h"

TArray<FVector2D> ULineChart::GetPointLocation()
{
	return PointLocations;
}

void ULineChart::OnPopulateMesh(const FGeometry& AllottedGeometry, TArray<FSlateVertex>& vertex, TArray<SlateIndex>& index)
{

	vertex.Empty();
	index.Empty();
	if (Data.Num() < 2)return;
	float stepW = Width / (Data.Num() - 1);
	AreaTop = FMath::Clamp(AreaTop, 0.f, 1.f);
	AreaBottom = FMath::Clamp(AreaBottom, 0.f, 1.f);
	AALineWidth = FMath::Clamp(AALineWidth, 0.f, 5.f);
	//Width
	
	TArray<FSlateVertex> Points;
	for (int i = 0; i < Data.Num(); i++)
	{
		Data[i] = FMath::Clamp(Data[i], 0.f, 1.f);
		FSlateVertex curVertext;
		curVertext.Position = FVector2D(FirstX + i * stepW, FirstY - Height * Data[i]);
		PointLocations.AddUnique(FVector2D(curVertext.Position.X+Width/2, Height - Height * Data[i]));
		/*if (IsBig && IsShowPoint)
		{
			DrawPoint(PointRidus + 100, curVertext.Position);
		}*/
		if (IsShowPoint)
		{
			DrawPoint(PointRidus, curVertext.Position);
		}
		Points.Add(curVertext);
	}

	if (IsCurve)
	{
		Points = PointList(Points, 100, FirstY, FirstY - Height);
	}
	if (IsBroken)
	{
		TArray<FSlateVertex> NewPoint;
		FSlateVertex LastPos = Points[0];
		NewPoint.Add(LastPos);
		for (int i = 0; i < Points.Num(); i++)
		{

			CheckPointXuXianLen(NewPoint, LastPos, Points[i]);

		}
		Points = NewPoint;
	}

	if (IsCurve && !IsBroken)
	{
		ThreeLines(stepW, 0, vertex, index, Points, 1);  
		ThreeLines(stepW, 0, vertex, index, Points, 2);  
		ThreeLines(stepW, 0, vertex, index, Points, 3);  
	}
	else
	{
		ZhangThreeLines(stepW, 0, vertex, index, Points, 1);
		ZhangThreeLines(stepW, 0, vertex, index, Points, 2);
		ZhangThreeLines(stepW, 0, vertex, index, Points, 3);

	}
	/*FSlateVertex FirstPos1;
	FSlateVertex FirstPos2;
	FSlateVertex Last1;
	FSlateVertex Last2;

	for (int i = 0; i < Points.Num() - 1; i++)
	{
		if (IsBroken && (i % 2 == 1))continue;
		FSlateVertex LinePoints[4];
		Points[i].Color = UseLineColor ? LineColor : GetColorByPos(Points[i].Position.X, Points[i].Position.Y);
		Points[i].Color.A = 255;

		GetRectPoint(LinePoints, Points[i].Position, Points[i + 1].Position, LineWidth);
		LinePoints[0].Color = UseLineColor ? LineColor : GetColorByPos(LinePoints[0].Position.X, LinePoints[0].Position.Y);
		LinePoints[1].Color = UseLineColor ? LineColor : GetColorByPos(LinePoints[1].Position.X, LinePoints[1].Position.Y);
		LinePoints[2].Color = UseLineColor ? LineColor : GetColorByPos(LinePoints[2].Position.X, LinePoints[2].Position.Y);
		LinePoints[3].Color = UseLineColor ? LineColor : GetColorByPos(LinePoints[3].Position.X, LinePoints[3].Position.Y);
		LinePoints[0].Color.A = 255;
		LinePoints[1].Color.A = 255;
		LinePoints[2].Color.A = 255;
		LinePoints[3].Color.A = 255;
		AddUIVertexQuad(LinePoints);
		if (IsBroken)continue;
		if (i == 0)
		{
			FirstPos1 = LinePoints[0];
			FirstPos2 = LinePoints[1];
			Last1 = LinePoints[2];
			Last2 = LinePoints[3];
		}
		else
		{
			int count = VertexData.Num();
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
	}*/

	if (IsArea)
	{
		for (int i = 0; i < Points.Num() - 1; i++)
		{
			FSlateVertex LinePoints[4];
			LinePoints[0].Position = Points[i].Position;
			LinePoints[1].Position = Points[i + 1].Position;
			LinePoints[2].Position = FVector2D(Points[i + 1].Position.X, FirstY);
			LinePoints[3].Position = FVector2D(Points[i].Position.X, FirstY);
			LinePoints[0].Color = GetColorByPos(LinePoints[0].Position.X, LinePoints[0].Position.Y);
			LinePoints[1].Color = GetColorByPos(LinePoints[1].Position.X, LinePoints[1].Position.Y);
			LinePoints[2].Color = LinePoints[1].Color;
			LinePoints[3].Color = LinePoints[0].Color;
			LinePoints[0].Color.A = (AreaBottom + (FirstY - LinePoints[0].Position.Y) / Height * (AreaTop - AreaBottom)) * 255;
			LinePoints[1].Color.A = (AreaBottom + (FirstY - LinePoints[1].Position.Y) / Height * (AreaTop - AreaBottom)) * 255;
			LinePoints[2].Color.A = 255 * AreaBottom;
			LinePoints[3].Color.A = 255 * AreaBottom;
			AddUIVertexQuad(LinePoints);
		}
	}
	ESlateDrawEffect CurDarwEffect;
	CurDarwEffect |= ESlateDrawEffect::NoPixelSnapping;
	SetDrawEffect(CurDarwEffect);
}
void ULineChart::ThreeLines(float x, float y, TArray<FSlateVertex>& vertex, TArray<SlateIndex>& index, TArray<FSlateVertex> Points, int pos)
{

	FSlateVertex FirstPos1;
	FSlateVertex FirstPos2;
	FSlateVertex Last1;
	FSlateVertex Last2;
	TArray<FSlateVertex> TempPoints;
	for (int i = 0; i < Points.Num() - 1; i++)
	{
		if (IsBroken && (i % 2 == 1))continue;
		FSlateVertex LinePoints[4];
		Points[i].Color = UseLineColor ? LineColor : GetColorByPos(Points[i].Position.X, Points[i].Position.Y);
		Points[i].Color.A = 255;
		if (pos == 1) {
			GetRectPointLine(LinePoints, Points[i].Position, Points[i + 1].Position, LineWidth, TempPoints, pos, AALineWidth);
			TempPoints.Empty();
			TempPoints.Add((LinePoints[2]));
			TempPoints.Add((LinePoints[3]));
			LinePoints[0].Color = UseLineColor ? LineColor : GetColorByPos(LinePoints[0].Position.X, LinePoints[0].Position.Y);
			LinePoints[1].Color = UseLineColor ? LineColor : GetColorByPos(LinePoints[1].Position.X, LinePoints[1].Position.Y);
			LinePoints[2].Color = UseLineColor ? LineColor : GetColorByPos(LinePoints[2].Position.X, LinePoints[2].Position.Y);
			LinePoints[3].Color = UseLineColor ? LineColor : GetColorByPos(LinePoints[3].Position.X, LinePoints[3].Position.Y);
			LinePoints[0].Color.A = 0;
			LinePoints[1].Color.A = 255;
			LinePoints[2].Color.A = 255;
			LinePoints[3].Color.A = 0;
		}
		else if (pos == 2)
		{
			GetRectPointLine(LinePoints, Points[i].Position, Points[i + 1].Position, LineWidth, TempPoints, pos, AALineWidth);
			TempPoints.Empty();
			TempPoints.Add((LinePoints[2]));
			TempPoints.Add((LinePoints[3]));
			LinePoints[0].Color = UseLineColor ? LineColor : GetColorByPos(LinePoints[0].Position.X, LinePoints[0].Position.Y);
			LinePoints[1].Color = UseLineColor ? LineColor : GetColorByPos(LinePoints[1].Position.X, LinePoints[1].Position.Y);
			LinePoints[2].Color = UseLineColor ? LineColor : GetColorByPos(LinePoints[2].Position.X, LinePoints[2].Position.Y);
			LinePoints[3].Color = UseLineColor ? LineColor : GetColorByPos(LinePoints[3].Position.X, LinePoints[3].Position.Y);
			LinePoints[0].Color.A = 255;
			LinePoints[1].Color.A = 0;
			LinePoints[2].Color.A = 0;
			LinePoints[3].Color.A = 255;
		}
		else
		{
			GetRectPointLine(LinePoints, Points[i].Position, Points[i + 1].Position, LineWidth, TempPoints, pos, 0);
			//TempPoints.Empty();
			//TempPoints.Add((LinePoints[2]));
			//TempPoints.Add((LinePoints[3]));
			LinePoints[0].Color = UseLineColor ? LineColor : GetColorByPos(LinePoints[0].Position.X, LinePoints[0].Position.Y);
			LinePoints[1].Color = UseLineColor ? LineColor : GetColorByPos(LinePoints[1].Position.X, LinePoints[1].Position.Y);
			LinePoints[2].Color = UseLineColor ? LineColor : GetColorByPos(LinePoints[2].Position.X, LinePoints[2].Position.Y);
			LinePoints[3].Color = UseLineColor ? LineColor : GetColorByPos(LinePoints[3].Position.X, LinePoints[3].Position.Y);
			//LinePoints[0].Color.A = 255;
			//LinePoints[1].Color.A = 255;
			//LinePoints[2].Color.A = 255;
			//LinePoints[3].Color.A = 255;
		}
		AddUIVertexQuad(LinePoints);
		if (IsBroken)continue;
		if (i == 0)
		{
			FirstPos1 = LinePoints[0];
			FirstPos2 = LinePoints[1];
			Last1 = LinePoints[2];
			Last2 = LinePoints[3];
		}
		else
		{
			int count = VertexData.Num();
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
	}

}

void ULineChart::ZhangThreeLines(float x, float y, TArray<FSlateVertex>& vertex, TArray<SlateIndex>& index, TArray<FSlateVertex> Points, int pos)
{

	FSlateVertex FirstPos1;
	FSlateVertex FirstPos2;
	FSlateVertex Last1;
	FSlateVertex Last2;
	TArray<FSlateVertex> TempPoints;
	for (int i = 0; i < Points.Num() - 1; i++)
	{
		if (IsBroken && (i % 2 == 1))continue;
		FSlateVertex LinePoints[4];
		Points[i].Color = UseLineColor ? LineColor : GetColorByPos(Points[i].Position.X, Points[i].Position.Y);
		Points[i].Color.A = 255;
		if (pos == 1) {
			GetRectPointLineByZhang(LinePoints, Points[i].Position, Points[i + 1].Position, LineWidth, TempPoints, pos, AALineWidth);
			TempPoints.Empty();
			TempPoints.Add((LinePoints[2]));
			TempPoints.Add((LinePoints[3]));
			LinePoints[0].Color = UseLineColor ? LineColor : GetColorByPos(LinePoints[0].Position.X, LinePoints[0].Position.Y);
			LinePoints[1].Color = UseLineColor ? LineColor : GetColorByPos(LinePoints[1].Position.X, LinePoints[1].Position.Y);
			LinePoints[2].Color = UseLineColor ? LineColor : GetColorByPos(LinePoints[2].Position.X, LinePoints[2].Position.Y);
			LinePoints[3].Color = UseLineColor ? LineColor : GetColorByPos(LinePoints[3].Position.X, LinePoints[3].Position.Y);
			LinePoints[0].Color.A = 0;
			LinePoints[1].Color.A = 255;
			LinePoints[2].Color.A = 255;
			LinePoints[3].Color.A = 0;
		}
		else if (pos == 2)
		{
			GetRectPointLineByZhang(LinePoints, Points[i].Position, Points[i + 1].Position, LineWidth, TempPoints, pos, AALineWidth);
			TempPoints.Empty();
			TempPoints.Add((LinePoints[2]));
			TempPoints.Add((LinePoints[3]));
			LinePoints[0].Color = UseLineColor ? LineColor : GetColorByPos(LinePoints[0].Position.X, LinePoints[0].Position.Y);
			LinePoints[1].Color = UseLineColor ? LineColor : GetColorByPos(LinePoints[1].Position.X, LinePoints[1].Position.Y);
			LinePoints[2].Color = UseLineColor ? LineColor : GetColorByPos(LinePoints[2].Position.X, LinePoints[2].Position.Y);
			LinePoints[3].Color = UseLineColor ? LineColor : GetColorByPos(LinePoints[3].Position.X, LinePoints[3].Position.Y);
			LinePoints[0].Color.A = 255;
			LinePoints[1].Color.A = 0;
			LinePoints[2].Color.A = 0;
			LinePoints[3].Color.A = 255;
		}
		else
		{
			GetRectPointLineByZhang(LinePoints, Points[i].Position, Points[i + 1].Position, LineWidth, TempPoints, pos, 0);
			//TempPoints.Empty();
			//TempPoints.Add((LinePoints[2]));
			//TempPoints.Add((LinePoints[3]));
			LinePoints[0].Color = UseLineColor ? LineColor : GetColorByPos(LinePoints[0].Position.X, LinePoints[0].Position.Y);
			LinePoints[1].Color = UseLineColor ? LineColor : GetColorByPos(LinePoints[1].Position.X, LinePoints[1].Position.Y);
			LinePoints[2].Color = UseLineColor ? LineColor : GetColorByPos(LinePoints[2].Position.X, LinePoints[2].Position.Y);
			LinePoints[3].Color = UseLineColor ? LineColor : GetColorByPos(LinePoints[3].Position.X, LinePoints[3].Position.Y);
			/*LinePoints[0].Color.A = 255;
			LinePoints[1].Color.A = 255;
			LinePoints[2].Color.A = 255;
			LinePoints[3].Color.A = 255;*/
		}
		AddUIVertexQuad(LinePoints);
		if (IsBroken)continue;
		if (i == 0)
		{
			FirstPos1 = LinePoints[0];
			FirstPos2 = LinePoints[1];
			Last1 = LinePoints[2];
			Last2 = LinePoints[3];
		}
		else
		{
			int count = VertexData.Num();
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
	}
}
TArray<FSlateVertex> ULineChart::PointList(TArray<FSlateVertex> points, int pointSize, float MaxY, float MinY)
{
	TArray<FSlateVertex> result;
	result.Add(points[0]);
	points.Insert(FSlateVertex(), 0);
	points[0].Position = points[1].Position + (points[1].Position - points[2].Position);
	points.Add(FSlateVertex());
	points[points.Num() - 1].Position = points[points.Num() - 2].Position + (points[points.Num() - 2].Position - points[points.Num() - 3].Position);
	int smoothAmount = points.Num() * pointSize;
	for (int index = 1; index <= smoothAmount; index++)
	{
		FSlateVertex curVert = Interp(points, (float)index / smoothAmount);
		curVert.Position.Y = FMath::Clamp(curVert.Position.Y, MinY, MaxY);
		result.Add(curVert);
	}
	return result;
}

FSlateVertex ULineChart::Interp(TArray<FSlateVertex>& pts, float t)
{
	int numSections = pts.Num() - 3;
	int currPt = FMath::Min(FMath::FloorToInt(t * (float)numSections), numSections - 1);
	float u = t * (float)numSections - (float)currPt;

	FVector2D a = pts[currPt].Position;
	FVector2D b = pts[currPt + 1].Position;
	FVector2D c = pts[currPt + 2].Position;
	FVector2D d = pts[currPt + 3].Position;
	FVector2D fin = 0.5f * (
		(-a + 3.f * b - 3.f * c + d) * (u * u * u)
		+ (2.f * a - 5.f * b + 4.f * c - d) * (u * u)
		+ (-a + c) * u
		+ 2.f * b
		);
	FSlateVertex result;
	result.Position = fin;
	return result;
}

void ULineChart::CheckPointXuXianLen(TArray<FSlateVertex>& NewPoint, FSlateVertex& LastPos, FSlateVertex NextPos)
{
	float len = 0;
	if (NewPoint.Num() % 2 == 0)
	{
		len = XuXianLen;
	}
	else
	{
		len = XuXianGap;
	}

	if (FVector2D::Distance(LastPos.Position, NextPos.Position) < len)
	{
		return;
	}

	LastPos.Position += (NextPos.Position - LastPos.Position).GetSafeNormal() * len;
	NewPoint.Add(LastPos);
	CheckPointXuXianLen(NewPoint, LastPos, NextPos);
}

void ULineChart::DrawPoint(float Ridius, FVector2D Origin)
{
	FVector2D prevOuter(Origin.X + Ridius, Origin.Y);
	FVector2D pos[3];
	FSlateVertex uiVertices[3];
	for (float i = 0; i <= 360; i++)
	{
		float rad = -FMath::DegreesToRadians(i);
		float c = FMath::Cos(rad);
		float s = FMath::Sin(rad);
		pos[0] = prevOuter;
		pos[1] = FVector2D(Origin.X + Ridius * c, Origin.Y + Ridius * s);
		pos[2] = Origin;
		//pos[3] = Origin;
		prevOuter = pos[1];
		for (int j = 0; j < 3; j++)
		{
			uiVertices[j].Position = pos[j];
			uiVertices[j].Color = GetColorByRadius(Ridius, 0, (j == 0 || j == 1) ? Ridius : 0);
			//uiVertices[j].Color.A = 1;
		}
		 //AddUIVertexQuad(uiVertices);
		int currentVertCount = VertexData.Num();
		for (int index = 0; index < 4; ++index)
		{
			VertexData.Add(uiVertices[index]);
		}
		IndexData.Add(currentVertCount);
		IndexData.Add(currentVertCount + 1);
		IndexData.Add(currentVertCount + 2);

		//IndexData.Add(currentVertCount + 2);
		//IndexData.Add(currentVertCount + 3);
		//IndexData.Add(currentVertCount);
	}
}


