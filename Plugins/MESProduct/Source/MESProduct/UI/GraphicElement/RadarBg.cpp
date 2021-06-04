// Fill out your copyright notice in the Description page of Project Settings.


#include "RadarBg.h"

void URadarBg::OnPopulateMesh(const FGeometry& AllottedGeometry, TArray<FSlateVertex>& vertex, TArray<SlateIndex>& index)
{
	vertex.Empty();
	index.Empty();
	if (EdgeCount < 3)
	{
		EdgeCount = 3;
	}
	if (EdgeCount > 100)
	{
		EdgeCount = 100;
	}
	if (StepCount < 1)
	{
		StepCount = 1;
	}
	if (StepCount > 20)
	{
		StepCount = 20;
	}
	if (LineWidth < 0)
	{
		LineWidth = 0;
	}
	outerRadius = (AllottedGeometry.Size.X < AllottedGeometry.Size.Y ? AllottedGeometry.Size.X : AllottedGeometry.Size.Y) * 0.5;
	float stepRadius = outerRadius / StepCount;
	if (LineWidth > stepRadius)
	{
		LineWidth = stepRadius;
	}
	float PreRaius = 0;//ǰһ�ΰ뾶
	for (int i = 0; i < StepCount; i++)
	{
		float CurRadius = PreRaius + stepRadius;
		if(StepColor)
		{
			
			DrawPolygon(PreRaius, CurRadius - LineWidth, GradiantColors[i%GradiantColors.Num()],true);	
		}
		else
		{
			DrawPolygon(PreRaius, CurRadius - LineWidth, GetColorByRadius(outerRadius,0,CurRadius),false);
		}
		DrawPolygon(CurRadius - LineWidth, CurRadius, LineColor,true);
		PreRaius = CurRadius;
	}
	if(ShowConnect)
	{
		float lineZhi = LineWidth * FMath::Cos(180.f / EdgeCount * FMath::DegreesToRadians(1.f));
		DrawConnectLine(outerRadius - lineZhi, lineZhi);
	}
}

void URadarBg::DrawPolygon(float inner, float outer,FColor color,bool LimitColor)
{
	float x = outer * FMath::Cos(0);
	float y = outer * FMath::Sin(0);
	FVector2D prevOuter(x, y);
	x = inner * FMath::Cos(0);
	y = inner * FMath::Sin(0);
	FVector2D prevInner(x, y);
	FVector2D pos[4];
	FSlateVertex uiVertices[4];
	int start = 0;
	int end = EdgeCount;
	float step = 360.f / EdgeCount;

	for (int i = start; i <= end; i++)
	{
		float rad = -FMath::DegreesToRadians(i * step + 90);
		float c = FMath::Cos(rad);
		float s = FMath::Sin(rad);

		pos[0] = prevOuter;
		pos[1] = FVector2D(outer * c, outer * s);
		pos[2] = FVector2D(inner * c, inner * s);
		pos[3] = prevInner;
		prevOuter = pos[1];
		prevInner = pos[2];
		if (i == start) continue;
		for (int j = 0; j < 4; j++)
		{
			if(LimitColor)
			{
				uiVertices[j].Color = color;	
			}
			else
			{
				if (GradiantMethod == EGradiantType::ZeroToFull)
				{
					uiVertices[j].Color = GetColorByAngle(0, 360, i * step);
				}
				else if(GradiantMethod == EGradiantType::InToOut)
				{
					uiVertices[j].Color = GetColorByRadius(outerRadius, 0,(j == 0 || j == 1)?outer:inner);
				}
				else
				{
					uiVertices[j].Color = GetColorByPos(pos[j].X,pos[j].Y);		
				}
			}
			uiVertices[j].Position = pos[j];
		}
		AddUIVertexQuad(uiVertices);
	}
}

void URadarBg::DrawConnectLine(float outer, float w)
{
	int start = 0;
	int end = EdgeCount;
	float step = 360.f / EdgeCount;
	FVector2D StartPos(0,0);
	for (int i = start; i <= end; i++)
	{
		float rad = -FMath::DegreesToRadians(i * step + 90);
		float c = FMath::Cos(rad);
		float s = FMath::Sin(rad);
		FVector2D EndPos(outer * c, outer * s);
		FSlateVertex v[4];
		GetRectPoint(v,StartPos, EndPos,w*0.5);
		v[0].Color = LineColor;
		v[1].Color = LineColor;
		v[2].Color = LineColor;
		v[3].Color = LineColor;
		AddUIVertexQuad(v);
	}
}
