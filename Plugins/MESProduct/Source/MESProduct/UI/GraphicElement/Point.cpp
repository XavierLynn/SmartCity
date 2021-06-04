// Fill out your copyright notice in the Description page of Project Settings.


#include "Point.h"

void UPoint::OnPopulateMesh(const FGeometry& AllottedGeometry, TArray<FSlateVertex>& vertex, TArray<SlateIndex>& index)
{
	vertex.Empty();
	index.Empty();
	float outer = (AllottedGeometry.Size.X < AllottedGeometry.Size.Y ? AllottedGeometry.Size.X : AllottedGeometry.Size.Y) * 0.5;

	if(AntiEdget < 1)
	{
		AntiEdget = 1;
	}
	if(AntiEdget > outer)
	{
		AntiEdget = outer;
	}
	if(OutLineWidth < 0)
	{
		OutLineWidth = 0;
	}
	if(OutLineWidth > outer  - AntiEdget)
	{
		OutLineWidth = outer - AntiEdget;
	}
	
	FColor OutColor = GradiantColors.Num() > 1 ? GradiantColors[1] : GradiantColors[0];
	DrawCircle( 0, outer - OutLineWidth - AntiEdget -1, GradiantColors[0], OutColor);
	DrawCircle( outer - OutLineWidth - AntiEdget -1,outer - OutLineWidth - AntiEdget, OutColor,OutLineColor);
	DrawCircle( outer - OutLineWidth - AntiEdget,outer - AntiEdget, OutLineColor, OutLineColor);
	DrawCircle( outer - AntiEdget,outer, OutLineColor, FColor(OutLineColor.R,OutLineColor.G,OutLineColor.B,0));
}

void UPoint::DrawCircle(float inner,float outer,FColor inColor, FColor ouColor)
{
	float x = outer * FMath::Cos(0);
	float y = outer * FMath::Sin(0);
	FVector2D prevOuter = FVector2D(x, y);

	x = inner * FMath::Cos(0);
	y = inner * FMath::Sin(0);
	FVector2D prevInner = FVector2D(x, y);

	FVector2D pos[4];
	FSlateVertex uiVertices[4];
	
	int start = 0;
	int end = 30;

	for (int i = start; i <= end; i++)
	{
		float rad = FMath::DegreesToRadians(i * 12.f);
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
			if (j == 0 || j == 1)
			{
				uiVertices[j].Color = ouColor;
			}
			else
			{
				uiVertices[j].Color = inColor;
			}
			uiVertices[j].Position = pos[j];
		}
		AddUIVertexQuad(uiVertices);
	}
}
