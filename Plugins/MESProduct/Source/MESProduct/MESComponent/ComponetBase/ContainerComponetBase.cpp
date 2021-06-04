// Fill out your copyright notice in the Description page of Project Settings.


#include "ContainerComponetBase.h"

TArray<UContainerComponetBase*> UContainerComponetBase::ConstainStackInMoving = TArray<UContainerComponetBase*>();

void UContainerComponetBase::AddInMovingStack()
{
	if (!ConstainStackInMoving.Contains(this))
	{
		ConstainStackInMoving.Add(this);
	}
	else
	{
		if (GetTopContainer() != this)
		{
			ConstainStackInMoving.Remove(this);
			ConstainStackInMoving.Add(this);
		}
	}
}

void UContainerComponetBase::RemoveInMovingStack()
{
	ConstainStackInMoving.Remove(this);
}

UContainerComponetBase* UContainerComponetBase::GetTopContainer()
{
	if (ConstainStackInMoving.Num() > 0)
	{
		return ConstainStackInMoving[ConstainStackInMoving.Num() - 1];
	}
	return nullptr;
}

void UContainerComponetBase::NativeDestruct()
{
	ConstainStackInMoving.Remove(this);
	Super::NativeDestruct();
}
