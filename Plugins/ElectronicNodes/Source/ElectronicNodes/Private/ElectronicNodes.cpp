/* Copyright (C) 2019 Hugo ATTAL - All Rights Reserved
 * This plugin is downloadable from the UE4 Marketplace
 */

#include "ElectronicNodes.h"
#include "ENConnectionDrawingPolicy.h"

#define LOCTEXT_NAMESPACE "FElectronicNodesModule"

void FElectronicNodesModule::StartupModule()
{
	TSharedPtr<FENConnectionDrawingPolicyFactory>ENConnectionFactory = MakeShareable(new FENConnectionDrawingPolicyFactory);
	FEdGraphUtilities::RegisterVisualPinConnectionFactory(ENConnectionFactory);
}

void FElectronicNodesModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FElectronicNodesModule, ElectronicNodes)