// Fill out your copyright notice in the Description page of Project Settings.


#include "RoundRect.h"

void URoundRect::OnPopulateMesh(const FGeometry& AllottedGeometry, TArray<FSlateVertex>& vertex, TArray<SlateIndex>& index)
{
	vertex.Empty();
	index.Empty();
	float HalfW = AllottedGeometry.Size.X / 2;
	float HalfH = AllottedGeometry.Size.Y / 2;
	radius = FixRadius;
	if (radius > HalfW)
	{
		radius = HalfW;
	}
	if (radius > HalfH)
	{
		radius = HalfH;
	}
	if (radius <= 0)
	{
		radius = 0.001f;
	}
	
	FSlateVertex Pos[4];
	//中间的矩形
	GetRectPoint(Pos, FVector2D(-HalfW, 0), FVector2D(HalfW, 0),HalfH - radius);
	SetColor(Pos);
	//上边的矩形
	GetRectPoint(Pos, FVector2D(-HalfW + radius,-HalfH + radius*0.5), FVector2D(HalfW - radius, -HalfH + radius*0.5), radius*0.5);
	SetColor(Pos);
	//下边的矩形
	GetRectPoint(Pos, FVector2D(-HalfW + radius,HalfH - radius*0.5), FVector2D(HalfW - radius, HalfH - radius*0.5), radius*0.5);
	SetColor(Pos);
	if(ShowLeftTop)
	{
		DrawAngle(FVector2D(-HalfW + radius, -HalfH + radius), 90, 180, radius, 2);	
	}
	else
	{
		GetRectPoint(Pos, FVector2D(-HalfW,-HalfH+radius*0.5), FVector2D(-HalfW + radius,-HalfH+radius*0.5), radius*0.5);
		SetColor(Pos);
	}
	if (ShowRightTop)
	{
		DrawAngle(FVector2D(HalfW - radius, -HalfH + radius), 0, 90, radius, 2);
	}
	else
	{
		GetRectPoint(Pos, FVector2D(HalfW - radius, -HalfH + radius * 0.5), FVector2D(HalfW, -HalfH + radius * 0.5), radius*0.5);
		SetColor(Pos);
	}

	if (ShowLeftBottom)
	{
		DrawAngle(FVector2D(-HalfW + radius, HalfH - radius), 180, 270, radius, 2);
	}
	else
	{
		GetRectPoint(Pos, FVector2D(-HalfW, HalfH - radius * 0.5), FVector2D(-HalfW + radius, HalfH - radius * 0.5), radius*0.5);
		SetColor(Pos);
	}
	
	if (ShowRightBottom)
	{
		DrawAngle(FVector2D(HalfW - radius, HalfH - radius), 270, 360, radius, 2);
	}
	else
	{
		GetRectPoint(Pos, FVector2D(HalfW - radius, HalfH - radius * 0.5), FVector2D(HalfW, HalfH - radius * 0.5), radius*0.5);
		SetColor(Pos);
	}
	
}

void URoundRect::SetColor(FSlateVertex pos[])
{
	for(int i = 0;i< 4;i++)
	{
		pos[i].Color = GetColorByPos(pos[i].Position.X, pos[i].Position.Y);
	}
	AddUIVertexQuad(pos);
}

void URoundRect::DrawAngle(FVector2D centerPos, float startAngle, float endAngle, float curRadius,float stepAngle)
{
	float x = curRadius * FMath::Cos(0);
	float y = curRadius * FMath::Sin(0);
	FVector2D prevOuter(x, y);
	FVector2D prevInner(0, 0);
	FVector2D pos[4];
	FSlateVertex uiVertices[4];

	for (int i = startAngle; i <= endAngle; i+= stepAngle)
	{
		float rad = -FMath::DegreesToRadians(i);
		float c = FMath::Cos(rad);
		float s = FMath::Sin(rad);

		pos[0] = prevOuter;
		pos[1] = FVector2D(curRadius * c, curRadius * s);
		pos[2] = FVector2D(0, 0);
		pos[3] = prevInner;
		prevOuter = pos[1];
		prevInner = pos[2];
		if (i == startAngle) continue;
		for (int j = 0; j < 4; j++)
		{
			uiVertices[j].Position = pos[j] + centerPos;
			uiVertices[j].Color = GetColorByPos(uiVertices[j].Position.X, uiVertices[j].Position.Y);
		}
		AddUIVertexQuad(uiVertices);
	}
}

