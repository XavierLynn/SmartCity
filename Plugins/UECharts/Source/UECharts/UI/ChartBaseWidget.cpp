// Fill out your copyright notice in the Description page of Project Settings.


#include "ChartBaseWidget.h"
#include "Slate/SMeshWidget.h"
#include "Slate/SlateVectorArtInstanceData.h"
#include "SlateMaterialBrush.h"


#define LOCTEXT_NAMESPACE "ChartElementPaletteCategory"
class SMyUIMeshWidget :public SMeshWidget
{
public:
	SLATE_BEGIN_ARGS(SMyUIMeshWidget) {}
	SLATE_END_ARGS()
public:
	
	void Construct(const FArguments& Args, UChartBaseWidget& Inthis)
	{
		MeshUI = &Inthis;
	}

	void SetR()
	{
		if (RenderData.Num() == 0)
		{
			RenderData.Add(FRenderData());
			EnableInstancing(0, 1);
		}
	}

	//绘制函数
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override 
	{

		if (MeshUI->GradiantColors.Num() == 0)
		{
			MeshUI->GradiantColors.Add(FColor::White);
		}
		float Scale = 1.f;
		float B = 1;
		float C = 1;
		float D = 1;
		//矩阵
		AllottedGeometry.GetAccumulatedRenderTransform().GetMatrix().GetMatrix(Scale, B, C, D);

		Scale = Scale < D ? Scale : D;

		MeshUI->FirstX = -AllottedGeometry.Size.X / 2;
		MeshUI->FirstY = AllottedGeometry.Size.Y / 2;
		MeshUI->Width = AllottedGeometry.Size.X;
		MeshUI->Height = AllottedGeometry.Size.Y;
		
		FVector2D TrailOriginWindowSpace = AllottedGeometry.LocalToAbsolute(AllottedGeometry.GetLocalSize() * 0.5f);

		MeshUI->PushUpdateVertext(TrailOriginWindowSpace, Scale);

		//确定绘画范围以及材质
		FSlateMaterialBrush brush = FSlateMaterialBrush(*(MeshUI->UIMaterial), FVector2D(256, 256));

		//绘画工具
		FSlateResourceHandle handle = FSlateApplication::Get().GetRenderer()->GetResourceHandle(brush);


		//添加顶点信息
		MeshUI->OnPopulateMesh(AllottedGeometry, MeshUI->VertexData, MeshUI->IndexData);



		for (int i = 0; i < MeshUI->VertexData.Num(); i++)
		{
			auto TexCoordsOne = (MeshUI->VertexData[i].Position.X - MeshUI->FirstX) / MeshUI->Width;
			auto TexCoordsTwo = (MeshUI->FirstY + MeshUI->VertexData[i].Position.Y) / MeshUI->Height;

			MeshUI->VertexData[i].TexCoords[0] = TexCoordsOne;
			MeshUI->VertexData[i].TexCoords[1] = TexCoordsTwo;

			FLinearColor lc = FLinearColor::FromSRGBColor(MeshUI->VertexData[i].Color);
			lc *= InWidgetStyle.GetColorAndOpacityTint();
			MeshUI->VertexData[i].Color = lc.ToFColor(true);
		}
		

		//DrawEffects |= ESlateDrawEffect::NoiPxelSnapping;
		
		MeshUI->DrawEffects |= ESlateDrawEffect::NoPixelSnapping;
		//开始绘画
		FSlateDrawElement::MakeCustomVerts(OutDrawElements, LayerId, handle, MeshUI->VertexData,
			MeshUI->IndexData, RenderData[0].PerInstanceBuffer.Get(),
			0, RenderData[0].PerInstanceBuffer->GetNumInstances(),MeshUI->DrawEffects);
		return LayerId;
		
	};
public:
	UChartBaseWidget* MeshUI;
};


//MeshUIBase 赋值 基础的材质
UChartBaseWidget::UChartBaseWidget()
{
	if (UIMaterial ==NULL)
	{
		static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialAsset(TEXT("/UECharts/Materials/ChartElment/MeshUIBaseMaterial.MeshUIBaseMaterial"));
		if (MaterialAsset.Succeeded())
		{
			UIMaterial = MaterialAsset.Object;
		}
	}
}

void UChartBaseWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	BaseMesh->SetR();
}

void UChartBaseWidget::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
}

TSharedRef<SWidget> UChartBaseWidget::RebuildWidget()
{
	if (BaseMesh != nullptr)
	{
		BaseMesh.Reset();
	}
	BaseMesh = SNew(SMyUIMeshWidget, *this);
	return BaseMesh.ToSharedRef();
}

void UChartBaseWidget::OnPopulateMesh(const FGeometry& AllottedGeometry, TArray<FSlateVertex>& vertex, TArray<SlateIndex>& index)
{
	vertex.Empty();
	index.Empty();
	float HalfW = AllottedGeometry.Size.X / 2;
	float HalfH = AllottedGeometry.Size.Y / 2;
	FSlateVertex F1;
	F1.Color = GradiantColors[0];
	F1.Position = FVector2D(-HalfW, HalfH);

	FSlateVertex F2;
	F2.Color = GradiantColors[0];
	F2.Position = FVector2D(HalfW, HalfH);

	FSlateVertex F3;
	F3.Color = GradiantColors[0];
	F3.Position = FVector2D(HalfW, -HalfH);

	FSlateVertex F4;
	F4.Color = GradiantColors[0];
	F4.Position = FVector2D(-HalfW, -HalfH);
	vertex.Add(F1);
	vertex.Add(F2);
	vertex.Add(F3);
	vertex.Add(F4);

	index.Add(0);
	index.Add(1);
	index.Add(2);

	index.Add(2);
	index.Add(3);
	index.Add(0);
}

void UChartBaseWidget::PushUpdateVertext(const FVector2D& Position, float Scale)
{
	FSlateVectorArtInstanceData Data;
	Data.SetPosition(Position);
	Data.SetScale(Scale);
	Data.SetBaseAddress(0);
	{
		FSlateInstanceBufferData PerInstanceData;
		PerInstanceData.Add(Data.GetData());
		if (BaseMesh != nullptr)
			BaseMesh->UpdatePerInstanceBuffer(0, PerInstanceData);
	}
}

FColor UChartBaseWidget::GetColorByPos(float x, float y)
{
	int Count = GradiantColors.Num();
	if (Count == 1)
	{
		return GradiantColors[0];
	}
	float dur = 1.f / (Count - 1);
	float t = 0;
	switch (GradiantStyle)
	{
	case EGradiantMethod::LeftToRight:
		t = (x - FirstX) / Width;
		break;
	case EGradiantMethod::UpToDown:
		t = (y + FirstY) / Height;
		break;
	default:
		return GradiantColors[0];
	}
	int index = (int)(t / dur); ;
	if (index >= Count - 1)
	{
		index = Count - 2;
	}
	t = (t - index * dur) / dur;
	if (index < 0)
	{
		return GradiantColors[0];
	}
	return Lerp(GradiantColors[index], GradiantColors[index + 1], t);
}

FColor UChartBaseWidget::GetColorByAngle(float startAngle, float EndAngle, float curAngle)
{
	int Count = GradiantColors.Num();
	if (Count == 1)
	{
		return GradiantColors[0];
	}
	float dur = 1.f / (Count - 1);
	float t = (curAngle - startAngle) / (EndAngle - startAngle);
	int index = (int)(t / dur); ;
	if (index >= Count - 1)
	{
		index = Count - 2;
	}
	t = (t - index * dur) / dur;
	if (index < 0)
	{
		return GradiantColors[0];
	}
	return Lerp(GradiantColors[index], GradiantColors[index + 1], t);
}

FColor UChartBaseWidget::GetColorByRadius(float Outer, float Inner, float CurRadius)
{
	int Count = GradiantColors.Num();
	if (Count == 1)
	{
		return GradiantColors[0];
	}
	float dur = 1.f / (Count - 1);
	float t = (CurRadius - Inner) / (Outer - Inner);
	int index = (int)(t / dur); ;
	if (index >= Count - 1)
	{
		index = Count - 2;
	}
	t = (t - index * dur) / dur;
	if (index < 0)
	{
		return GradiantColors[0];
	}
	return Lerp(GradiantColors[index], GradiantColors[index + 1], t);
}

void UChartBaseWidget::GetRectPoint(FSlateVertex verts[], FVector2D p1, FVector2D p2, float lineThickness)
{
	if (p2.X == p1.X)//��ֱ����
	{
		int Add = (p2.Y >= p1.Y) ? 1 : -1;
		verts[0].Position = FVector2D(p1.X - lineThickness * Add, p1.Y);
		verts[1].Position = FVector2D(p1.X + lineThickness * Add, p1.Y);
		verts[2].Position = FVector2D(p2.X + lineThickness * Add, p2.Y);
		verts[3].Position = FVector2D(p2.X - lineThickness * Add, p2.Y);
		return;
	}
	if (p2.Y == p1.Y)//ˮƽ����
	{
		int Add = (p2.X >= p1.X) ? 1 : -1;
		verts[0].Position = FVector2D(p1.X, p1.Y - lineThickness * Add);
		verts[1].Position = FVector2D(p1.X, p1.Y + lineThickness * Add);
		verts[2].Position = FVector2D(p2.X, p2.Y + lineThickness * Add);
		verts[3].Position = FVector2D(p2.X, p2.Y - lineThickness * Add);
		return;
	}
	//ͨ��б�����߶�
	float k = (p2.Y - p1.Y) / (p2.X - p1.X);
	float A = FMath::Atan(k);
	float ofssetX = lineThickness * FMath::Sin(A);
	float offsetY = lineThickness;
	if (k != 0)
	{
		offsetY = ofssetX / k;
	}
	if (p2.X > p1.X)
	{
		verts[0].Position = FVector2D(p1.X + ofssetX, p1.Y - offsetY);
		verts[1].Position = FVector2D(p1.X - ofssetX, p1.Y + offsetY);
		verts[2].Position = FVector2D(p2.X - ofssetX, p2.Y + offsetY);
		verts[3].Position = FVector2D(p2.X + ofssetX, p2.Y - offsetY);
	}
	else
	{
		verts[0].Position = FVector2D(p1.X - ofssetX, p1.Y + offsetY);
		verts[1].Position = FVector2D(p1.X + ofssetX, p1.Y - offsetY);
		verts[2].Position = FVector2D(p2.X + ofssetX, p2.Y - offsetY);
		verts[3].Position = FVector2D(p2.X - ofssetX, p2.Y + offsetY);
	}
}


void UChartBaseWidget::AddUIVertexQuad(FSlateVertex uiVertices[])
{
	int currentVertCount = VertexData.Num();
	for (int index = 0; index < 4; ++index)
	{
		VertexData.Add(uiVertices[index]);
	}
	IndexData.Add(currentVertCount);
	IndexData.Add(currentVertCount + 1);
	IndexData.Add(currentVertCount + 2);

	IndexData.Add(currentVertCount + 2);
	IndexData.Add(currentVertCount + 3);
	IndexData.Add(currentVertCount);
}



void UChartBaseWidget::SetDrawEffect(ESlateDrawEffect InDrawEffects /*= ESlateDrawEffect::None*/)
{
	DrawEffects = InDrawEffects;
}

void UChartBaseWidget::Print_C(FString InString)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, InString);
	}
}

FColor UChartBaseWidget::Lerp(FColor a, FColor b, float t)
{
	FLinearColor A = FLinearColor::FromSRGBColor(a);
	FLinearColor B = FLinearColor::FromSRGBColor(b);
	FLinearColor C = A + (B - A) * t;

	return C.ToFColor(true);
}

#if WITH_EDITOR
const FText UChartBaseWidget::GetPaletteCategory()
{
	return LOCTEXT("ChartElementPaletteCategory", "ChartElement");
}

#endif
