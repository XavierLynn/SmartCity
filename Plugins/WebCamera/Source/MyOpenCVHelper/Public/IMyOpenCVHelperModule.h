/************************************************************************/
/* Author: YWT20                                                        */
/* Expected release year : 2020                                         */
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "HAL/IConsoleManager.h" // for TAutoConsoleVariable<>

class IConsoleVariable;

/**
 * 
 */
class IMyOpenCVHelperModule : public IModuleInterface
{
public:
	/** Virtual destructor. */
	virtual ~IMyOpenCVHelperModule() {}
};

