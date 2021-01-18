// Fill out your copyright notice in the Description page of Project Settings.


#include "CircleXL.h"

UCircleXL::UCircleXL()
{
	
}

void UCircleXL::OnPopulateMesh(const FGeometry& AllottedGeometry, TArray<FSlateVertex>& vertex, TArray<SlateIndex>& index)
{
	vertex.Empty();
	index.Empty();
	float outer = (AllottedGeometry.Size.X < AllottedGeometry.Size.Y ? AllottedGeometry.Size.X : AllottedGeometry.Size.Y) * 0.5;
	//环形比较特殊，所有要重新赋值他的宽高，避免渐变初故障
	FirstX = -outer;
	FirstY = outer;
	Width = outer*2;
	Height = Width;
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
	DrawCircle(outer, inner, Start, end, 0, 0,false,false);
	/*DrawCircle(outer + 1, outer, Start, end, 0, 0, true,false);
	DrawCircle(inner, inner - 1, Start, end, 0, 0,false,true);*/
	return;
}

void UCircleXL::DrawCircle(float outer, float inner, float start, float end, float offsetX, float offsetY, bool OuterAnti,bool innerAnti)
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