// Fill out your copyright notice in the Description page of Project Settings.


#include "MeshUIBase.h"

#include "Slate/SMeshWidget.h"
#include "Slate/SlateVectorArtInstanceData.h"
#include "Rendering/DrawElements.h"
#include "Styling/SlateBrush.h"
#include "Materials/MaterialInterface.h"
#include "Modules/ModuleManager.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Framework/Application/SlateApplication.h"
#include "SlateMaterialBrush.h"
#include "Runtime/SlateRHIRenderer/Public/Interfaces/ISlateRHIRendererModule.h"
#include "UObject/ConstructorHelpers.h"
#include "Slate/SMeshWidget.h"

#define LOCTEXT_NAMESPACE "MEChartElement"


class SUIMeshWidget : public SMeshWidget
{
public:
	SLATE_BEGIN_ARGS(SUIMeshWidget) { }
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& Args, UMeshUIBase& InThis)
	{
		This = &InThis;
	}

	void SetR()
	{
		if (RenderData.Num() == 0)
		{
			RenderData.Add(FRenderData());
			EnableInstancing(0, 1);
		}
	}

	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyClippingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override
	{
		if (This->GradiantColors.Num() == 0)
		{
			This->GradiantColors.Add(FColor::White);
		}
		float Scale = 1.f;
		float B = 1;
		float C = 1;
		float D = 1;
		AllottedGeometry.GetAccumulatedRenderTransform().GetMatrix().GetMatrix(Scale, B, C, D);
		Scale = Scale < D?Scale:D;
		This->FirstX = -AllottedGeometry.Size.X / 2;
		This->FirstY = AllottedGeometry.Size.Y / 2;
		This->Width = AllottedGeometry.Size.X;
		This->Height = AllottedGeometry.Size.Y;
		
		FVector2D TrailOriginWindowSpace = AllottedGeometry.LocalToAbsolute(AllottedGeometry.GetLocalSize() * 0.5f);
		This->PushUpdateVertext(TrailOriginWindowSpace, Scale);
		FSlateMaterialBrush brush = FSlateMaterialBrush(*(This->Materrial), FVector2D(256, 256));
		FSlateResourceHandle handle = FSlateApplication::Get().GetRenderer()->GetResourceHandle(brush);
		This->OnPopulateMesh(AllottedGeometry, This->VertexData, This->IndexData);
		for (int i = 0; i < This->VertexData.Num(); i++)
		{
			This->VertexData[i].TexCoords[0] = (This->VertexData[i].Position.X - This->FirstX) / This->Width;
			This->VertexData[i].TexCoords[1] = (This->FirstY + This->VertexData[i].Position.Y) / This->Height;
			FLinearColor lc = FLinearColor::FromSRGBColor(This->VertexData[i].Color);
			lc *= InWidgetStyle.GetColorAndOpacityTint();
			This->VertexData[i].Color = lc.ToFColor(true);
		}
		//绘制顶点数据
		FSlateDrawElement::MakeCustomVerts(OutDrawElements, LayerId, handle, This->VertexData,
			This->IndexData, RenderData[0].PerInstanceBuffer.Get(),
			0, RenderData[0].PerInstanceBuffer->GetNumInstances());
		return LayerId;
	}
public:
	UMeshUIBase* This;
};

void UMeshUIBase::OnPopulateMesh(const FGeometry& AllottedGeometry,TArray<FSlateVertex>& vertex, TArray<SlateIndex>& index)
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

void UMeshUIBase::PushUpdateVertext(const FVector2D& Position, float Scale)
{
	FSlateVectorArtInstanceData Data;
	Data.SetPosition(Position);
	Data.SetScale(Scale);
	Data.SetBaseAddress(0);

	{
		FSlateInstanceBufferData PerInstanceData;
		PerInstanceData.Add(Data.GetData());
		if(MyMesh != nullptr)
			MyMesh->UpdatePerInstanceBuffer(0, PerInstanceData);
	}
}

FColor UMeshUIBase::GetColorByAngle(float startAngle, float EndAngle, float curAngle)
{
	int Count = GradiantColors.Num();
	if (Count == 1)
	{
		return GradiantColors[0];
	}
	float dur = 1.f / (Count - 1);
	float t = (curAngle - startAngle)/(EndAngle - startAngle);
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

FColor UMeshUIBase::GetColorByRadius(float Outer, float Inner, float CurRadius)
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

void UMeshUIBase::GetRectPoint(FSlateVertex verts[],FVector2D p1, FVector2D p2, float lineThickness)
{
	if (p2.X == p1.X)//垂直方向
	{
		int Add = (p2.Y >= p1.Y)?1:-1;
		verts[0].Position = FVector2D(p1.X - lineThickness * Add, p1.Y);
		verts[1].Position = FVector2D(p1.X + lineThickness * Add, p1.Y);
		verts[2].Position = FVector2D(p2.X + lineThickness * Add, p2.Y);
		verts[3].Position = FVector2D(p2.X - lineThickness * Add, p2.Y);
		return;
	}
	if (p2.Y == p1.Y)//水平方向
	{
		int Add = (p2.X >= p1.X)?1:-1;
		verts[0].Position = FVector2D(p1.X, p1.Y - lineThickness * Add);
		verts[1].Position = FVector2D(p1.X, p1.Y + lineThickness * Add);
		verts[2].Position = FVector2D(p2.X, p2.Y + lineThickness * Add);
		verts[3].Position = FVector2D(p2.X, p2.Y - lineThickness * Add);
		return;
	}
	//通过斜率求线段
	float k = (p2.Y - p1.Y) / (p2.X - p1.X);
	float A = FMath::Atan(k);
	float ofssetX = lineThickness * FMath::Sin(A);
	float offsetY = lineThickness;
	if(k != 0)
	{
		offsetY = ofssetX / k;	
	}
	if(p2.X > p1.X)
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

FColor UMeshUIBase::GetColorByPos(float x, float y)
{
	int Count = GradiantColors.Num();
	if(Count == 1)
	{
		return GradiantColors[0];
	}
	float dur = 1.f / (Count - 1);
	float t = 0;
	switch(GradiantMethod)
	{
	case EGradiantType::LeftToRight:
		t = (x - FirstX) / Width;
		break;
	case EGradiantType::UpToDown:
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
	return Lerp(GradiantColors[index],GradiantColors[index + 1],t);
}

void UMeshUIBase::AddUIVertexQuad(FSlateVertex uiVertices[])
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

UMeshUIBase::UMeshUIBase()
{
	if (Materrial == NULL)
	{
		static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialAsset(TEXT("/MESProduct/Componet/ChartElment/MeshUIBaseMaterial.MeshUIBaseMaterial"));
		
		if (MaterialAsset.Succeeded())
		{
			Materrial = MaterialAsset.Object;
		}
	}
}

void UMeshUIBase::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	MyMesh->SetR();
}

void UMeshUIBase::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	if (MyMesh != nullptr)
	{
		MyMesh.Reset();
	}
}

TSharedRef<SWidget> UMeshUIBase::RebuildWidget()
{
	if (MyMesh != nullptr)
	{
		MyMesh.Reset();
	}
	MyMesh = SNew(SUIMeshWidget, *this);
	return MyMesh.ToSharedRef();
}

FColor UMeshUIBase::Lerp(FColor a, FColor b, float t)
{
	FLinearColor A = FLinearColor::FromSRGBColor(a);
	FLinearColor B = FLinearColor::FromSRGBColor(b);
	FLinearColor C = A + (B - A)*t;

	return C.ToFColor(true);
}

#if WITH_EDITOR

#include "UMGStyle.h"
const FText UMeshUIBase::GetPaletteCategory()
{
	return LOCTEXT("MEChartElementPaletteCategory", "MEChartElement");
}

#endif

#undef LOCTEXT_NAMESPACE