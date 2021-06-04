// Fill out your copyright notice in the Description page of Project Settings.


#include "RoundBorder.h"
#include "Blueprint\WidgetLayoutLibrary.h"

void URoundBorder::OnPopulateMesh(const FGeometry& AllottedGeometry, TArray<FSlateVertex>& vertex, TArray<SlateIndex>& index)
{
	vertex.Empty();
	index.Empty();
	float HalfW = AllottedGeometry.Size.X / 2;
	float HalfH = AllottedGeometry.Size.Y / 2;
	float radius = LineWidth / MainUIScale;

	FSlateVertex Pos[4];

	for (int i = 0; i < 4; i++)
	{
		Pos[i].Color = GradiantColors[0];
	}
	//上边的矩形
	GetRectPoint(Pos, FVector2D(-HalfW + radius, -HalfH + radius * 0.5), FVector2D(HalfW - radius, -HalfH + radius * 0.5), radius*0.5);
	AddUIVertexQuad(Pos);
	//下边的矩形
	GetRectPoint(Pos, FVector2D(-HalfW + radius, HalfH - radius * 0.5), FVector2D(HalfW - radius, HalfH - radius * 0.5), radius*0.5);
	AddUIVertexQuad(Pos);
	//左边的矩形
	GetRectPoint(Pos, FVector2D(-HalfW + radius * 0.5, HalfH), FVector2D(-HalfW + radius * 0.5, -HalfH), radius*0.5);
	AddUIVertexQuad(Pos);
	//右边的矩形
	GetRectPoint(Pos, FVector2D(HalfW - radius * 0.5, HalfH), FVector2D(HalfW - radius * 0.5, -HalfH), radius*0.5);
	AddUIVertexQuad(Pos);
}
