/* Copyright (C) 2019 Hugo ATTAL - All Rights Reserved
 * This plugin is downloadable from the UE4 Marketplace
 */

#pragma once

#include "CoreMinimal.h"
#include "EdGraphUtilities.h"
#include "ConnectionDrawingPolicy.h"
#include "../Public/ElectronicNodesSettings.h"

#include "BlueprintConnectionDrawingPolicy.h"
#include "MaterialGraph/MaterialGraphSchema.h"
#include "EdGraphSchema_K2.h"

struct ENRibbonConnection
{
	float Main;
	float Sub;
	bool Horizontal;
	float Start;
	float End;
	int32 Depth = 0;

	ENRibbonConnection(float Main, float Sub, bool Horizontal, float Start, float End, int32 Depth = 0)
	{
		this->Main = Main;
		this->Sub = Sub;
		this->Horizontal = Horizontal;
		this->Start = Start;
		this->End = End;
		this->Depth = Depth;
	}
};

struct FENConnectionDrawingPolicyFactory : public FGraphPanelPinConnectionFactory
{
public:
	virtual ~FENConnectionDrawingPolicyFactory() {}

	virtual class FConnectionDrawingPolicy* CreateConnectionPolicy(const class UEdGraphSchema* Schema, int32 InBackLayerID, int32 InFrontLayerID, float ZoomFactor, const class FSlateRect& InClippingRect, class FSlateWindowElementList& InDrawElements, class UEdGraph* InGraphObj) const override;

};

class FENConnectionDrawingPolicy : public FKismetConnectionDrawingPolicy
{
public:
	FENConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float ZoomFactor, const FSlateRect & InClippingRect, FSlateWindowElementList & InDrawElements, UEdGraph* InGraphObj)
		:FKismetConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, ZoomFactor, InClippingRect, InDrawElements, InGraphObj)
	{
	}

	virtual void DrawConnection(int32 LayerId, const FVector2D & Start, const FVector2D & End, const FConnectionParams & Params) override;

private:
	const UElectronicNodesSettings& ElectronicNodesSettings = *GetDefault<UElectronicNodesSettings>();
	bool ReversePins;
	float MinXOffset;
	float ClosestDistanceSquared;
	FVector2D ClosestPoint;
	TArray<ENRibbonConnection> RibbonConnections;
	TMap<FVector2D, int> PinsOffset;

	void ENCorrectZoomDisplacement(FVector2D& Start, FVector2D& End);
	void ENProcessRibbon(int32 LayerId, FVector2D& Start, FVector2D& End, const FConnectionParams& Params);
	void ENDrawSimpleCircuit(int32 LayerId, const FVector2D & Start, const FVector2D & End, FConnectionParams Params);
	void ENDrawComplexCircuit(int32 LayerId, const FVector2D & Start, const FVector2D & End, FConnectionParams Params);

	void ENDrawSpline(int32 LayerId, const FVector2D & Start, const FVector2D & StartTangent, const FVector2D & End, const FVector2D & EndTangent, const FConnectionParams & Params);

	void ENDrawBubbles(int32 LayerId, const FVector2D & Start, const FVector2D & StartTangent, const FVector2D & End, const FVector2D & EndTangent, const FConnectionParams & Params);
	void ENDrawArrow(int32 LayerId, const FVector2D & Start, const FVector2D & End, const FConnectionParams & Params);
	void ENComputeClosestPoint(const FVector2D & Start, const FVector2D & End);
};
