// Fill out your copyright notice in the Description page of Project Settings.


#include "ChartsAxis.h"

void UChartsAxis::OnPopulateMesh(const FGeometry& AllottedGeometry, TArray<FSlateVertex>& vertex, TArray<SlateIndex>& index)
{
	vertex.Empty();
	index.Empty();
	KeDuPoistion.Empty();
	if(KeDuNum < 2)
	{
		return;
	}
	
	FSlateVertex Pos[4];
	Pos[0].Color = GradiantColors[0];
	Pos[1].Color = GradiantColors[0];
	Pos[2].Color = GradiantColors[0];
	Pos[3].Color = GradiantColors[0];
	Pos[0].Position = FVector2D(FirstX, FirstY);
	Pos[1].Position = FVector2D(-FirstX, FirstY);
	Pos[2].Position = FVector2D(-FirstX, -FirstY);
	Pos[3].Position = FVector2D(FirstX, -FirstY);
	AddUIVertexQuad(Pos);
	float StepW = Width / (KeDuNum - 1);
	float StepH = Height / (KeDuNum - 1);
	
	FSlateVertex KeDuPos[4];
	KeDuPos[0].Color = GradiantColors[0];
	KeDuPos[1].Color = GradiantColors[0];
	KeDuPos[2].Color = GradiantColors[0];
	KeDuPos[3].Color = GradiantColors[0];
	if(Height > Width)
	{
		if(KeDuArrow != EAxisInterArrow::Left && KeDuArrow != EAxisInterArrow::Right)
		{
			KeDuArrow = EAxisInterArrow::Right;
		}
		for (int i = 0; i < KeDuNum; i++)
		{
			float Add = KeDuArrow == EAxisInterArrow::Left ? 1 : -1;
			FVector2D StartPos = FVector2D( -FirstX *Add,FirstY - i * StepH);
			FVector2D EndPos = FVector2D((FirstX - KeDuKuan)* Add,FirstY - i * StepH);
			GetRectPoint(KeDuPos, StartPos, EndPos, Width * 0.5f);
			AddUIVertexQuad(KeDuPos);
			KeDuPoistion.Add(EndPos);
		}
	}
	else
	{
		if (KeDuArrow != EAxisInterArrow::Up && KeDuArrow != EAxisInterArrow::Down)
		{
			KeDuArrow = EAxisInterArrow::Up;
		}
		for (int i = 0; i < KeDuNum; i++)
		{
			float Add = KeDuArrow == EAxisInterArrow::Down ? 1 : -1;
			FVector2D StartPos = FVector2D(FirstX + i * StepW, -FirstY *Add);
			FVector2D EndPos = FVector2D(FirstX + i * StepW, (FirstY  + KeDuKuan)* Add);
			GetRectPoint(KeDuPos, StartPos, EndPos, Height * 0.5f);
			AddUIVertexQuad(KeDuPos);
			KeDuPoistion.Add(EndPos);
		}
	}
}
