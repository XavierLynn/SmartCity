/* Copyright (C) 2019 Hugo ATTAL - All Rights Reserved
 * This plugin is downloadable from the UE4 Marketplace
 */

#pragma once

#include "Runtime/Engine/Classes/Engine/DeveloperSettings.h"
#include "ElectronicNodesSettings.generated.h"

UENUM(BlueprintType)
enum class EWireStyle : uint8
{
	Desactivated,
	SimpleCircuit,
	ComplexCircuit
};

UENUM(BlueprintType)
enum class EWireStyleForExec : uint8
{
	Same,
	SimpleCircuit,
	ComplexCircuit
};

UENUM(BlueprintType)
enum class EBubbleMovementStyle : uint8
{
	Linear,
	Cubic
};

UCLASS(config = EditorPerProjectUserSettings, meta = (DisplayName = "Electronic Nodes Plugin"))
class ELECTRONICNODES_API UElectronicNodesSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UElectronicNodesSettings()
	{
		CategoryName = TEXT("Plugins");
		SectionName = TEXT("Electronic Nodes Plugin");
	}

	/* Wire style of graph. "Deactivate" will totally shutdown the plugin and fallback to previous behaviour. */
	UPROPERTY(config, EditAnywhere, Category = "Wire Style")
		EWireStyle WireStyle = EWireStyle::ComplexCircuit;

	/* Specific wire style for exec wires. Default: same */
	UPROPERTY(config, EditAnywhere, Category = "Wire Style")
		EWireStyleForExec WireStyleForExec = EWireStyleForExec::Same;

	/* Round radius of the wires. Default: 10 */
	UPROPERTY(config, EditAnywhere, Category = "Wire Style")
		uint32 RoundRadius = 10;

	/* Horizontal offset of wires from nodes. Default: 16 */
	UPROPERTY(config, EditAnywhere, Category = "Wire Style")
		uint32 HorizontalOffset = 16;

	/* Fix default zoomed-out wire displacement. Default: true */
	UPROPERTY(config, EditAnywhere, Category = "Wire Style")
		bool FixZoomDisplacement = true;

	/* Activate ribbon cables for overlapping wires. */
	UPROPERTY(config, EditAnywhere, Category = "Ribbon Style (experimental)")
		bool ActivateRibbon = false;

	/* Offset between ribbon wires. Default: 4 */
	UPROPERTY(config, EditAnywhere, Category = "Ribbon Style (experimental)")
		uint32 RibbonOffset = 4;

	/* Offset of wires when merge into ribbon. Default: 20 */
	UPROPERTY(config, EditAnywhere, Category = "Ribbon Style (experimental)")
		uint32 RibbonMergeOffset = 20;

	/* Show moving bubbles on the wires. */
	UPROPERTY(config, EditAnywhere, Category = "Bubbles Style")
		bool ForceDrawBubbles = false;

	/* Bubbles movement style on the wires. */
	UPROPERTY(config, EditAnywhere, Category = "Bubbles Style")
		EBubbleMovementStyle BubbleMovementStyle = EBubbleMovementStyle::Linear;

	/* Size of bubbles on the wires. Default: 2.0 */
	UPROPERTY(config, EditAnywhere, Category = "Bubbles Style", meta = (ClampMin = "1.0"))
		float BubbleSize = 2.0f;

	/* Speed of bubbles on the wires. Default: 4.0 */
	UPROPERTY(config, EditAnywhere, Category = "Bubbles Style", meta = (ClampMin = "0.0"))
		float BubbleSpeed = 4.0f;

	/* Space between bubbles on the wires. Default: 20.0 */
	UPROPERTY(config, EditAnywhere, Category = "Bubbles Style", meta = (ClampMin = "10.0"))
		float BubbleSpace = 20.0f;

	virtual FName GetContainerName() const override
	{
		return "Editor";
	}
};