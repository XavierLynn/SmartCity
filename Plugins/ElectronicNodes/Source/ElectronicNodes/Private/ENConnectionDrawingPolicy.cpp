/* Copyright (C) 2019 Hugo ATTAL - All Rights Reserved
 * This plugin is downloadable from the UE4 Marketplace
 */

#include "ENConnectionDrawingPolicy.h"
#include "Misc/App.h"
#include "BlueprintEditorSettings.h"
#include "Editor.h"

FConnectionDrawingPolicy* FENConnectionDrawingPolicyFactory::CreateConnectionPolicy(const class UEdGraphSchema* Schema, int32 InBackLayerID, int32 InFrontLayerID, float ZoomFactor, const class FSlateRect& InClippingRect, class FSlateWindowElementList& InDrawElements, class UEdGraph* InGraphObj) const
{
	const UElectronicNodesSettings& ElectronicNodesSettings = *GetDefault<UElectronicNodesSettings>();
	if (ElectronicNodesSettings.WireStyle == EWireStyle::Desactivated)
	{
		return nullptr;
	}

	if (Schema->IsA(UEdGraphSchema_K2::StaticClass()) || Schema->IsA(UMaterialGraphSchema::StaticClass()))
	{
		return new FENConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, ZoomFactor, InClippingRect, InDrawElements, InGraphObj);
	}
	return nullptr;
}

void FENConnectionDrawingPolicy::DrawConnection(int32 LayerId, const FVector2D & Start, const FVector2D & End, const FConnectionParams & Params)
{
	FVector2D P0 = Start;
	FVector2D P1 = End;


	ENCorrectZoomDisplacement(P0, P1);
	ENProcessRibbon(LayerId, P0, P1, Params);
	
	MinXOffset = ElectronicNodesSettings.HorizontalOffset * ZoomFactor;
	ClosestDistanceSquared = MAX_FLT;

	if (Params.AssociatedPin1->PinType.PinCategory.ToString() == "exec" && ElectronicNodesSettings.WireStyleForExec != EWireStyleForExec::Same)
	{
		switch (ElectronicNodesSettings.WireStyleForExec)
		{
		case EWireStyleForExec::SimpleCircuit:
			ENDrawSimpleCircuit(LayerId, P0, P1, Params);
			break;
		case EWireStyleForExec::ComplexCircuit:
			ENDrawComplexCircuit(LayerId, P0, P1, Params);
			break;
		}
	}
	else
	{
		switch (ElectronicNodesSettings.WireStyle)
		{
		case EWireStyle::SimpleCircuit:
			ENDrawSimpleCircuit(LayerId, P0, P1, Params);
			break;
		case EWireStyle::ComplexCircuit:
			ENDrawComplexCircuit(LayerId, P0, P1, Params);
			break;
		}
	}

	

	if (Settings->bTreatSplinesLikePins)
	{
		const float QueryDistanceTriggerThresholdSquared = FMath::Square(Settings->SplineHoverTolerance + Params.WireThickness * 0.5f);
		const bool bCloseToSpline = ClosestDistanceSquared < QueryDistanceTriggerThresholdSquared;

		if (bCloseToSpline)
		{
			if (ClosestDistanceSquared < SplineOverlapResult.GetDistanceSquared())
			{
				const float SquaredDistToPin1 = (Params.AssociatedPin1 != nullptr) ? (P0 - ClosestPoint).SizeSquared() : FLT_MAX;
				const float SquaredDistToPin2 = (Params.AssociatedPin2 != nullptr) ? (P1 - ClosestPoint).SizeSquared() : FLT_MAX;

				SplineOverlapResult = FGraphSplineOverlapResult(Params.AssociatedPin1, Params.AssociatedPin2, ClosestDistanceSquared, SquaredDistToPin1, SquaredDistToPin2);
			}
		}
	}
}

void FENConnectionDrawingPolicy::ENDrawSimpleCircuit(int32 LayerId, const FVector2D & Start, const FVector2D & End, FConnectionParams Params)
{
	FVector2D P0 = Start;
	FVector2D P1 = End;

	const float RoundRadius = ElectronicNodesSettings.RoundRadius * ZoomFactor;
	float WireDirection = 1.0f;

	if (Params.StartDirection == Params.EndDirection)
	{
		ReversePins = (P1.X - P0.X < 0.0f);

		if (Params.StartDirection == EGPD_Input)
		{
			WireDirection = -1.0f;
		}
	}
	else
	{
		ReversePins = (P1.X - P0.X < 2 * RoundRadius + 2 * MinXOffset);
	}

	if (ReversePins)
	{
		P0 = End;
		P1 = Start;
	}

	const float YMultiplier = (P0.Y < P1.Y ? 1.0f : -1.0f);
	FVector2D VM = FVector2D(1.0f, YMultiplier) * WireDirection;

	const float TN = 2*FMath::Sqrt(2.0f) - 1;
	const float TD = 4.0f / TN;

	const FVector2D ForwardTangent = FVector2D(TN * ElectronicNodesSettings.RoundRadius, 0.0f) * ZoomFactor * WireDirection;
	const FVector2D DownTangent = FVector2D(0.0f, TN * ElectronicNodesSettings.RoundRadius) * ZoomFactor * YMultiplier * WireDirection;
	const FVector2D NullTangent = FVector2D(0.0f, 0.0f);

	if (P1.X - P0.X < 2 * MinXOffset + 2 * RoundRadius)
	{
		if (FMath::Abs(P1.Y - P0.Y) < 4 * RoundRadius)
		{
			ENDrawSpline(LayerId, P0, -ForwardTangent * TD, P1, -ForwardTangent * TD, Params);
			return;
		}
	}

	if (Params.StartDirection == Params.EndDirection)
	{
		if (Params.StartDirection == EGPD_Input)
		{
			ReversePins = !ReversePins;
			FVector2D TempPoint = P0;
			P0 = P1;
			P1 = TempPoint;
		}

		/*
		*   S┐ ┌──E
		*    │ │
		* E──┘ └S
		*/

		const FVector2D P2 = P0 + VM * FVector2D(FMath::Abs(P1.X - P0.X) + MinXOffset, 0.0f);
		const FVector2D P3 = P2 + VM * FVector2D(RoundRadius, RoundRadius);
		const FVector2D P5 = P1 + VM * FVector2D(MinXOffset, 0.0f);
		const FVector2D P4 = P5 + VM * FVector2D(RoundRadius, -RoundRadius);

		ENDrawSpline(LayerId, P0, NullTangent, P2, NullTangent, Params);
		ENDrawSpline(LayerId, P2, ForwardTangent, P3, DownTangent, Params);
		ENDrawSpline(LayerId, P3, NullTangent, P4, NullTangent, Params);
		ENDrawSpline(LayerId, P4, DownTangent, P5, -ForwardTangent, Params);
		ENDrawSpline(LayerId, P5, NullTangent, P1, NullTangent, Params);

		return;
	}

	if (ReversePins && Params.StartDirection == EGPD_Output)
	{
		if (FMath::Abs(P0.Y - P1.Y) < 4 * RoundRadius)
		{
			const FVector2D P2 = P0 + FVector2D(-MinXOffset, 0.0f);
			const FVector2D P7 = P1 + FVector2D(MinXOffset, 0.0f);
			ENDrawSpline(LayerId, P0, NullTangent, P2, NullTangent, Params);
			ENDrawSpline(LayerId, P7, NullTangent, P1, NullTangent, Params);

			if (YMultiplier > 0)
			{
				/*
				 * ┌──┐
				 * └E │
				 *   S┘
				*/

				const FVector2D P3 = P2 + FVector2D(0.0f, -2.0f * RoundRadius);
				const FVector2D P4 = P3 + FVector2D(P1.X - P0.X + 2 * MinXOffset, 0.0f);
				const FVector2D P5 = P4 + FVector2D(RoundRadius, RoundRadius);
				const FVector2D P6 = P7 + FVector2D(RoundRadius, -RoundRadius);

				ENDrawSpline(LayerId, P2, -ForwardTangent * TD, P3, ForwardTangent * TD, Params);
				ENDrawSpline(LayerId, P3, NullTangent, P4, NullTangent, Params);
				ENDrawSpline(LayerId, P4, ForwardTangent, P5, DownTangent, Params);
				ENDrawSpline(LayerId, P5, NullTangent, P6, NullTangent, Params);
				ENDrawSpline(LayerId, P6, DownTangent, P7, -ForwardTangent, Params);
			}
			else
			{
				/* 
				 * ┌──┐
				 * │ S┘
				 * └E
				*/

				const FVector2D P3 = P2 + FVector2D(-RoundRadius, -RoundRadius);
				//const FVector2D P7 = P1 + FVector2D(MinXOffset, 0.0f);
				const FVector2D P6 = P7 + FVector2D(0.0f, -2.0f * RoundRadius);
				const FVector2D P5 = P6 + FVector2D(P0.X - P1.X - 2 * MinXOffset, 0.0f);
				const FVector2D P4 = P5 + FVector2D(-RoundRadius, RoundRadius);

				ENDrawSpline(LayerId, P2, -ForwardTangent, P3, DownTangent, Params);
				ENDrawSpline(LayerId, P3, NullTangent, P4, NullTangent, Params);
				ENDrawSpline(LayerId, P4, DownTangent, P5, ForwardTangent, Params);
				ENDrawSpline(LayerId, P5, NullTangent, P6, NullTangent, Params);
				ENDrawSpline(LayerId, P6, ForwardTangent * TD, P7, -ForwardTangent * TD, Params);
			}
		}
		else
		{
			/*
			 *   E┐
			 *    │
			 * ┌──┘
			 * └S
			*/

			float OffsetX = FMath::Min(0.0f, P1.X - P0.X + 2.0f * MinXOffset);

			const FVector2D P2 = P0 + VM * FVector2D(-MinXOffset + OffsetX, 0.0f);
			const FVector2D P3 = P2 + VM * FVector2D(0.0f, 2.0f * RoundRadius);
			const FVector2D P4 = P3 + VM * FVector2D(P1.X - P0.X + 2.0f * MinXOffset - OffsetX, 0.0f);
			const FVector2D P5 = P4 + VM * FVector2D(RoundRadius, RoundRadius);

			const FVector2D P7 = P1 + VM * FVector2D(MinXOffset, 0.0f);
			const FVector2D P6 = P7 + VM * FVector2D(RoundRadius, -RoundRadius);

			ENDrawSpline(LayerId, P0, NullTangent, P2, NullTangent, Params);
			ENDrawSpline(LayerId, P2, -ForwardTangent * TD, P3, ForwardTangent * TD, Params);
			ENDrawSpline(LayerId, P3, NullTangent, P4, NullTangent, Params);
			ENDrawSpline(LayerId, P4, ForwardTangent, P5, DownTangent, Params);
			ENDrawSpline(LayerId, P5, NullTangent, P6, NullTangent, Params);
			ENDrawSpline(LayerId, P6, DownTangent, P7, -ForwardTangent, Params);
			ENDrawSpline(LayerId, P7, NullTangent, P1, NullTangent, Params);	
		}
	}
	else
	{
		/*
		 * S──┐
		 *    │
		 *    └E
		*/

		const FVector2D P5 = P1 + VM * FVector2D(-MinXOffset, 0.0f);
		const FVector2D P4 = P5 + VM * FVector2D(-RoundRadius, -RoundRadius);
		const FVector2D P3 = P4 + VM * FVector2D(0.0f, -FMath::Abs(P1.Y-P0.Y) + 2 * RoundRadius);
		const FVector2D P2 = P3 + VM * FVector2D(-RoundRadius, -RoundRadius);

		ENDrawSpline(LayerId, P0, NullTangent, P2, NullTangent, Params);

		if (FMath::Abs(P0.Y - P1.Y) > 2 * RoundRadius)
		{
			ENDrawSpline(LayerId, P2, ForwardTangent, P3, DownTangent, Params);
			ENDrawSpline(LayerId, P3, NullTangent, P4, NullTangent, Params);
			ENDrawSpline(LayerId, P4, DownTangent, P5, ForwardTangent, Params);
		}
		else
		{
			ENDrawSpline(LayerId, P2, ForwardTangent, P5, ForwardTangent, Params);
		}

		ENDrawSpline(LayerId, P5, NullTangent, P1, NullTangent, Params);
	}

}

void FENConnectionDrawingPolicy::ENDrawComplexCircuit(int32 LayerId, const FVector2D & Start, const FVector2D & End, FConnectionParams Params)
{
	FVector2D P0 = Start;
	FVector2D P1 = End;

	const float RoundRadius = ElectronicNodesSettings.RoundRadius * ZoomFactor;
	float WireDirection = 1.0f;

	if (Params.StartDirection == Params.EndDirection)
	{
		ReversePins = (P1.X - P0.X < 0.0f);

		if (Params.StartDirection == EGPD_Input)
		{
			WireDirection = -1.0f;
		}
	}
	else
	{
		ReversePins = (P1.X - P0.X < 2 * RoundRadius + 2 * MinXOffset);
	}

	if (ReversePins)
	{
		P0 = End;
		P1 = Start;
	}

	const float YMultiplier = (P0.Y < P1.Y ? 1.0f : -1.0f);
	FVector2D VM = FVector2D(1.0f, YMultiplier) * WireDirection;

	const float TN = 2 * FMath::Sqrt(2.0f) - 1;
	const float TD = 4.0f / TN;
	const float TS = 0.55166f / TN;

	const FVector2D ForwardTangent = FVector2D(TN * ElectronicNodesSettings.RoundRadius, 0.0f) * ZoomFactor * WireDirection;
	const FVector2D DiagTangent = ForwardTangent.GetRotated(45 * YMultiplier) * ZoomFactor;
	const FVector2D DownTangent = FVector2D(0.0f, TN * ElectronicNodesSettings.RoundRadius) * ZoomFactor * YMultiplier * WireDirection;
	const FVector2D NullTangent = FVector2D(0.0f, 0.0f);

	const float MaxDiagRadius = RoundRadius * FMath::Sqrt(2.0f) / 2.0f;
	const float MinDiagRadius = RoundRadius * (1.0f - (FMath::Sqrt(2.0f) / 2.0f));

	if (P1.X - P0.X < 2 * MinXOffset + 2 * RoundRadius)
	{
		if (FMath::Abs(P1.Y - P0.Y) < 4 * RoundRadius)
		{
			ENDrawSpline(LayerId, P0, -ForwardTangent * TD, P1, -ForwardTangent * TD, Params);
			return;
		}
	}

	if (Params.StartDirection == Params.EndDirection)
	{
		float DiagSize = (P1.X - P0.X);

		if (Params.StartDirection == EGPD_Input)
		{
			ReversePins = !ReversePins;
			FVector2D TempPoint = P0;
			P0 = P1;
			P1 = TempPoint;
		}
		const FVector2D P2 = P0 + VM * FVector2D(MinXOffset, 0.0f);
		const FVector2D P7 = P1 + VM * FVector2D(MinXOffset, 0.0f);

		ENDrawSpline(LayerId, P0, NullTangent, P2, NullTangent, Params);
		ENDrawSpline(LayerId, P7, NullTangent, P1, NullTangent, Params);

		if (FMath::Abs(P0.Y - P1.Y) < DiagSize + 2 * RoundRadius)
		{
			DiagSize = FMath::Abs(P1.Y - P0.Y) - 2 * RoundRadius;

			const FVector2D P6 = P7 + VM * FVector2D(RoundRadius, -RoundRadius);
			const FVector2D P5 = P6 + VM * FVector2D(-MinDiagRadius, -MaxDiagRadius);
			const FVector2D P4 = P5 + VM * FVector2D(-DiagSize, -DiagSize);
			const FVector2D P3 = P4 + VM * FVector2D(-MaxDiagRadius, -MinDiagRadius);

			ENDrawSpline(LayerId, P2, NullTangent, P3, NullTangent, Params);
			ENDrawSpline(LayerId, P3, ForwardTangent * TS, P4, DiagTangent * TS, Params);
			ENDrawSpline(LayerId, P4, NullTangent, P5, NullTangent, Params);
			ENDrawSpline(LayerId, P5, DiagTangent * TS, P6, DownTangent * TS, Params);
			ENDrawSpline(LayerId, P6, DownTangent, P7, -ForwardTangent, Params);

		}
		else
		{
			const FVector2D P3 = P2 + VM * FVector2D(MaxDiagRadius, MinDiagRadius);
			const FVector2D P4 = P3 + VM * FVector2D(DiagSize, DiagSize);
			const FVector2D P5 = P4 + VM * FVector2D(MinDiagRadius, MaxDiagRadius);
			const FVector2D P6 = P7 + VM * FVector2D(RoundRadius, -RoundRadius);

			ENDrawSpline(LayerId, P2, ForwardTangent * TS, P3, DiagTangent * TS, Params);
			ENDrawSpline(LayerId, P3, NullTangent, P4, NullTangent, Params);
			ENDrawSpline(LayerId, P4, DiagTangent * TS, P5, DownTangent * TS, Params);
			ENDrawSpline(LayerId, P5, NullTangent, P6, NullTangent, Params);
			ENDrawSpline(LayerId, P6, DownTangent, P7, -ForwardTangent, Params);	
		}

		return;
	}

	if (ReversePins && Params.StartDirection == EGPD_Output)
	{
		if (FMath::Abs(P0.Y - P1.Y) < 4 * RoundRadius)
		{
			const FVector2D P2 = P0 + VM * FVector2D(-MinXOffset, 0.0f);
			const FVector2D P8 = P1 + VM * FVector2D(MinXOffset, 0.0f);

			ENDrawSpline(LayerId, P0, NullTangent, P2, NullTangent, Params);
			ENDrawSpline(LayerId, P8, NullTangent, P1, NullTangent, Params);

			float DiagSize = FMath::Abs(P1.Y - P0.Y);

			if (YMultiplier > 0)
			{
				const FVector2D P3 = P2 + FVector2D(0.0f, -2.0f * RoundRadius);
				const FVector2D P7 = P8 + FVector2D(RoundRadius, -RoundRadius);
				const FVector2D P6 = P7 + FVector2D(-MinDiagRadius, -MaxDiagRadius);
				const FVector2D P5 = P6 + FVector2D(-DiagSize, -DiagSize);
				const FVector2D P4 = P5 + FVector2D(-MaxDiagRadius, -MinDiagRadius);

				ENDrawSpline(LayerId, P2, -ForwardTangent * TD, P3, ForwardTangent * TD, Params);
				ENDrawSpline(LayerId, P2, NullTangent, P3, NullTangent, Params);
				ENDrawSpline(LayerId, P4, ForwardTangent * TS, P5, DiagTangent * TS, Params);
				ENDrawSpline(LayerId, P5, NullTangent, P6, NullTangent, Params);
				ENDrawSpline(LayerId, P6, DiagTangent * TS, P7, DownTangent * TS, Params);
				ENDrawSpline(LayerId, P7, DownTangent, P8, -ForwardTangent, Params);
			}
			else
			{
				const FVector2D P3 = P2 + FVector2D(-RoundRadius, -RoundRadius);
				const FVector2D P4 = P3 + FVector2D(MinDiagRadius, -MaxDiagRadius);
				const FVector2D P5 = P4 + FVector2D(DiagSize, -DiagSize);
				const FVector2D P6 = P5 + FVector2D(MaxDiagRadius, -MinDiagRadius);
				const FVector2D P7 = P8 + FVector2D(0.0f, -2.0f * RoundRadius);

				ENDrawSpline(LayerId, P2, -ForwardTangent, P3, DownTangent, Params);
				ENDrawSpline(LayerId, P3, DownTangent * TS, P4, DiagTangent * TS, Params);
				ENDrawSpline(LayerId, P4, NullTangent, P5, NullTangent, Params);
				ENDrawSpline(LayerId, P5, DiagTangent * TS, P6, ForwardTangent * TS, Params);
				ENDrawSpline(LayerId, P6, NullTangent, P7, NullTangent, Params);
				ENDrawSpline(LayerId, P7, ForwardTangent * TD, P8, -ForwardTangent * TD, Params);
			}
		}
		else
		{
			float DiagSize = P1.X - P0.X + 2 * MinXOffset + 2 * MaxDiagRadius;

			if (DiagSize > FMath::Abs(P1.Y - P0.Y) - (2 * RoundRadius + 2 * MaxDiagRadius))
			{
				DiagSize = FMath::Abs(P1.Y - P0.Y) - (4 * RoundRadius);

				const float XOffset = FMath::Max(0.0f,DiagSize - (P1.X - P0.X + 2 * MinXOffset - RoundRadius));

				const FVector2D P2 = P0 + VM * FVector2D(-MinXOffset - XOffset, 0.0f);
				const FVector2D P3 = P2 + VM * FVector2D(0.0f, 2 * RoundRadius);
				const FVector2D P8 = P1 + VM * FVector2D(MinXOffset, 0.0f);
				const FVector2D P7 = P8 + VM * FVector2D(RoundRadius, -RoundRadius);
				const FVector2D P6 = P7 + VM * FVector2D(-MinDiagRadius, -MaxDiagRadius);
				const FVector2D P5 = P6 + VM * FVector2D(-DiagSize, -DiagSize);
				const FVector2D P4 = P5 + VM * FVector2D(-MaxDiagRadius, -MinDiagRadius);

				ENDrawSpline(LayerId, P0, NullTangent, P2, NullTangent, Params);
				ENDrawSpline(LayerId, P2, -ForwardTangent * TD, P3, ForwardTangent * TD, Params);
				ENDrawSpline(LayerId, P3, NullTangent, P4, NullTangent, Params);
				ENDrawSpline(LayerId, P4, ForwardTangent * TS, P5, DiagTangent * TS, Params);
				ENDrawSpline(LayerId, P5, NullTangent, P6, NullTangent, Params);
				ENDrawSpline(LayerId, P6, DiagTangent * TS, P7, DownTangent * TS, Params);
				ENDrawSpline(LayerId, P7, DownTangent, P8, -ForwardTangent, Params);
				ENDrawSpline(LayerId, P8, NullTangent, P1, NullTangent, Params);

			}
			else
			{

				const FVector2D P2 = P0 + VM * FVector2D(-MinXOffset, 0.0f);
				const FVector2D P3 = P2 + VM * FVector2D(-RoundRadius, RoundRadius);
				const FVector2D P4 = P3 + VM * FVector2D(MinDiagRadius, MaxDiagRadius);
				const FVector2D P5 = P4 + VM * FVector2D(DiagSize, DiagSize);
				const FVector2D P6 = P5 + VM * FVector2D(MinDiagRadius, MaxDiagRadius);
				const FVector2D P8 = P1 + VM * FVector2D(MinXOffset, 0.0f);
				const FVector2D P7 = P8 + VM * FVector2D(RoundRadius, -RoundRadius);

				ENDrawSpline(LayerId, P0, NullTangent, P2, NullTangent, Params);
				ENDrawSpline(LayerId, P2, -ForwardTangent, P3, DownTangent, Params);
				ENDrawSpline(LayerId, P3, DownTangent * TS, P4, DiagTangent * TS, Params);
				ENDrawSpline(LayerId, P4, NullTangent, P5, NullTangent, Params);
				ENDrawSpline(LayerId, P5, DiagTangent * TS, P6, DownTangent * TS, Params);
				ENDrawSpline(LayerId, P6, NullTangent, P7, NullTangent, Params);
				ENDrawSpline(LayerId, P7, DownTangent, P8, -ForwardTangent, Params);
				ENDrawSpline(LayerId, P8, NullTangent, P1, NullTangent, Params);
			}
		}
		return;
	}
	
	if ((P1.X - P0.X) > (FMath::Abs(P1.Y - P0.Y) + 2 * MinXOffset + RoundRadius))
	{
		const float DiagSize = FMath::Abs(P1.Y - P0.Y) - 2 * MinDiagRadius;

		const FVector2D P5 = P1 + VM * FVector2D(-MinXOffset, 0.0f);
		const FVector2D P4 = P5 + VM * FVector2D(-MaxDiagRadius, -MinDiagRadius);
		const FVector2D P3 = P4 + VM * FVector2D(-DiagSize, -DiagSize);
		const FVector2D P2 = P3 + VM * FVector2D(-MaxDiagRadius, -MinDiagRadius);

		ENDrawSpline(LayerId, P0, NullTangent, P2, NullTangent, Params);
		if (FMath::Abs(P0.Y - P1.Y) > 2 * MinDiagRadius)
		{
			ENDrawSpline(LayerId, P2, ForwardTangent * TS, P3, DiagTangent * TS, Params);
			ENDrawSpline(LayerId, P3, NullTangent, P4, NullTangent, Params);
			ENDrawSpline(LayerId, P4, DiagTangent * TS, P5, ForwardTangent * TS, Params);
		}
		else
		{
			ENDrawSpline(LayerId, P2, ForwardTangent * TS, P5, ForwardTangent * TS, Params);
		}
		ENDrawSpline(LayerId, P5, NullTangent, P1, NullTangent, Params);

	}
	else if ((P1.X - P0.X) > 2 * MinXOffset + 2 * RoundRadius)
	{
		const float DiagSize = (P1.X - P0.X) - (2 * MinXOffset + 2 * RoundRadius);
			
		const FVector2D P2 = P0 + VM * FVector2D(MinXOffset, 0.0f);
		const FVector2D P3 = P2 + VM * FVector2D(MaxDiagRadius, MinDiagRadius);
		const FVector2D P4 = P3 + VM * FVector2D(DiagSize, DiagSize);
		const FVector2D P5 = P4 + VM * FVector2D(MinDiagRadius, MaxDiagRadius);

		const FVector2D P7 = P1 + VM * FVector2D(- MinXOffset, 0.0f);
		const FVector2D P6 = P7 + VM * FVector2D(- RoundRadius, - RoundRadius);

		ENDrawSpline(LayerId, P0, NullTangent, P2, NullTangent, Params);
		ENDrawSpline(LayerId, P2, ForwardTangent * TS, P3, DiagTangent * TS, Params);
		ENDrawSpline(LayerId, P3, NullTangent, P4, NullTangent, Params);
		if ((P5.Y - P6.Y) * YMultiplier < 0)
		{
			ENDrawSpline(LayerId, P4, DiagTangent * TS, P5, DownTangent * TS, Params);
			ENDrawSpline(LayerId, P5, NullTangent, P6, NullTangent, Params);
			ENDrawSpline(LayerId, P6, DownTangent, P7, ForwardTangent, Params);
		}
		else
		{
			ENDrawSpline(LayerId, P4, DiagTangent * TS, P7, ForwardTangent * TS, Params);
		}
		ENDrawSpline(LayerId, P7, NullTangent, P1, NullTangent, Params);
	}
	else
	{
		ENDrawSimpleCircuit(LayerId, P0, P1, Params);
	}
}

void FENConnectionDrawingPolicy::ENDrawSpline(int32 LayerId, const FVector2D & Start, const FVector2D & StartTangent, const FVector2D & End, const FVector2D & EndTangent, const FConnectionParams & Params)
{
	FSlateDrawElement::MakeDrawSpaceSpline(DrawElementsList, LayerId,
		Start, StartTangent, End, EndTangent,
		Params.WireThickness, ESlateDrawEffect::None, Params.WireColor);

	ENDrawBubbles(LayerId, Start, StartTangent, End, EndTangent, Params);

	if (StartTangent == EndTangent)
	{
		ENDrawArrow(LayerId, Start, End, Params);
		ENComputeClosestPoint(Start, End);
	}
}

void FENConnectionDrawingPolicy::ENCorrectZoomDisplacement(FVector2D& Start, FVector2D& End)
{
	if (ElectronicNodesSettings.FixZoomDisplacement)
	{
		const float ZoomDisplacement = ZoomFactor * -19.0f + 8.0f;
		if (ZoomDisplacement > 0)
		{
			Start.X += ZoomDisplacement / 2.0f;
			End.X -= ZoomDisplacement / 2.0f;
		}
	}
}

void FENConnectionDrawingPolicy::ENProcessRibbon(int32 LayerId, FVector2D& Start, FVector2D& End, const FConnectionParams& Params)
{
	int32 DepthOffsetX = 0;
	int32 DepthOffsetY = 0;
	const float SplineSize = (End - Start).Size();

	if (ElectronicNodesSettings.ActivateRibbon)
	{
		for (ENRibbonConnection RibbonConnection : RibbonConnections)
		{
			if (RibbonConnection.Horizontal)
			{
				if (FMath::Abs(Start.Y - RibbonConnection.Main) < ElectronicNodesSettings.RibbonOffset)
				{
					const float CurrentMax = FMath::Max(Start.X, End.X);
					const float CurrentMin = FMath::Min(Start.X, End.X);
					const float RibbonMax = FMath::Max(RibbonConnection.Start, RibbonConnection.End);
					const float RibbonMin = FMath::Min(RibbonConnection.Start, RibbonConnection.End);

					if (FMath::Min(CurrentMax, RibbonMax) > FMath::Max(CurrentMin, RibbonMin) - 1.0f)
					{
						if (End.Y - RibbonConnection.Sub > 0)
						{
							DepthOffsetY = FMath::Max(DepthOffsetY, FMath::Max(1, RibbonConnection.Depth + 1));
						}
						else
						{
							DepthOffsetY = FMath::Min(DepthOffsetY, FMath::Min(-1, RibbonConnection.Depth - 1));
						}
					}
				}
			}
		}
		for (ENRibbonConnection RibbonConnection : RibbonConnections)
		{
			if (!RibbonConnection.Horizontal)
			{
				if (FMath::Abs(End.X - RibbonConnection.Main) < ElectronicNodesSettings.RibbonOffset)
				{
					const float CurrentMax = FMath::Max(Start.Y, End.Y);
					const float CurrentMin = FMath::Min(Start.Y, End.Y);
					const float RibbonMax = FMath::Max(RibbonConnection.Start, RibbonConnection.End);
					const float RibbonMin = FMath::Min(RibbonConnection.Start, RibbonConnection.End);

					if (FMath::Min(CurrentMax, RibbonMax) > FMath::Max(CurrentMin, RibbonMin) - 1.0f)
					{
						if ((Start.Y - RibbonConnection.Start)*FMath::Sign(End.Y - Start.Y) > 0)
						{
							DepthOffsetX = FMath::Max(DepthOffsetX, FMath::Max(1, RibbonConnection.Depth + 1));
						}
						else
						{
							DepthOffsetX = FMath::Min(DepthOffsetX, FMath::Min(-1, RibbonConnection.Depth - 1));
						}

						if (DepthOffsetY != 0)
						{
							DepthOffsetX = FMath::Sign(End.Y - Start.Y) * DepthOffsetY;
						}
					}
				}
			}
		}

		RibbonConnections.Add(ENRibbonConnection(Start.Y, End.Y, true, Start.X, End.X, DepthOffsetY));
		RibbonConnections.Add(ENRibbonConnection(End.X, Start.X, false, Start.Y, End.Y, DepthOffsetX));

		FVector2D StartKey(FMath::FloorToInt(Start.X), FMath::FloorToInt(Start.Y));
		FVector2D EndKey(FMath::FloorToInt(End.X), FMath::FloorToInt(End.Y));

		if (DepthOffsetY != 0)
		{
			FVector2D newStart = Start;
			newStart.X += ElectronicNodesSettings.RibbonMergeOffset * ZoomFactor;
			newStart.Y += (int32)ElectronicNodesSettings.RibbonOffset * DepthOffsetY * ZoomFactor;

			ENDrawSimpleCircuit(LayerId, Start, newStart, Params);

			Start = newStart;
		}

		if (DepthOffsetX != 0)
		{
			FVector2D newEnd = End;
			newEnd.X -= (int32)ElectronicNodesSettings.RibbonOffset * DepthOffsetX * ZoomFactor;

			ENDrawSimpleCircuit(LayerId, newEnd, End, Params);

			End = newEnd;
		}
	}
}

void FENConnectionDrawingPolicy::ENDrawBubbles(int32 LayerId, const FVector2D & Start, const FVector2D & StartTangent, const FVector2D & End, const FVector2D & EndTangent, const FConnectionParams & Params)
{
	if (Params.bDrawBubbles || ElectronicNodesSettings.ForceDrawBubbles)
	{
		FInterpCurve<float> SplineReparamTable;
		const float SplineLength = (Start - End).Size();
		const int32 NumBubbles = FMath::CeilToInt(SplineLength / ElectronicNodesSettings.BubbleSpace);

		const float BubbleSpeed = ElectronicNodesSettings.BubbleSpeed;
		const FVector2D BubbleSize = BubbleImage->ImageSize * ZoomFactor * 0.1f * ElectronicNodesSettings.BubbleSize * FMath::Sqrt(Params.WireThickness);
		const float Time = (FPlatformTime::Seconds() - GStartTime);

		float AlphaOffset = FMath::Frac(Time * BubbleSpeed);
		if (ReversePins)
		{
			AlphaOffset = 1.0f - AlphaOffset;
		}

		for (int32 i = 0; i < NumBubbles; ++i)
		{
			const float Alpha = (AlphaOffset + i) / NumBubbles;
			FVector2D BubblePos;
			if (ElectronicNodesSettings.BubbleMovementStyle == EBubbleMovementStyle::Cubic)
			{
				BubblePos = FMath::CubicInterp(Start, StartTangent, End, EndTangent, Alpha);
			}
			else
			{
				BubblePos = FMath::Lerp(Start, End, Alpha);
			}
			BubblePos -= (BubbleSize * 0.5f);

			FSlateDrawElement::MakeBox(
				DrawElementsList,
				LayerId,
				FPaintGeometry(BubblePos, BubbleSize, ZoomFactor),
				BubbleImage,
				ESlateDrawEffect::None,
				Params.WireColor
			);
		}
	}
}

void FENConnectionDrawingPolicy::ENDrawArrow(int32 LayerId, const FVector2D& Start, const FVector2D& End, const FConnectionParams& Params)
{
	if (MidpointImage != nullptr && (Start - End).Size() > 4 * MinXOffset)
	{
		const FVector2D MidpointDrawPos = (Start + End) / 2.0f - MidpointRadius * 0.75f;
		const FVector2D SlopeUnnormalized = (End - Start) * (ReversePins ? -1.0f : 1.0f);
		const float AngleInRadians = FMath::Atan2(SlopeUnnormalized.Y, SlopeUnnormalized.X);

		FSlateDrawElement::MakeRotatedBox(DrawElementsList, LayerId, FPaintGeometry(MidpointDrawPos, MidpointImage->ImageSize * ZoomFactor * 0.75f, ZoomFactor * 0.75f),
			MidpointImage, ESlateDrawEffect::None, AngleInRadians, TOptional<FVector2D>(), FSlateDrawElement::RelativeToElement, Params.WireColor);
	}
}

void FENConnectionDrawingPolicy::ENComputeClosestPoint(const FVector2D & Start, const FVector2D & End)
{
	const FVector2D TemporaryPoint = FMath::ClosestPointOnSegment2D(LocalMousePosition, Start, End);
	const float TemporaryDistance = (LocalMousePosition - TemporaryPoint).SizeSquared();

	if (TemporaryDistance < ClosestDistanceSquared)
	{
		ClosestDistanceSquared = TemporaryDistance;
		ClosestPoint = TemporaryPoint;
	}
}
