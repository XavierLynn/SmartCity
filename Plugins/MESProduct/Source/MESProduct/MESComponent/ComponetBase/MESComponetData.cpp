// Fill out your copyright notice in the Description page of Project Settings.


#include "MESComponetData.h"
#include "MESProduct\MESComponent\ComponetBase\MESPointComponetBase.h"
// Add default functionality here for any IDoubleInterface functions that are not pure virtual.

void UMESComponetData::BindGetComponetData(FUpdateDataSingleDelegate CallBack)
{
	if (actorBase != nullptr)
	{
		actorBase->BindGetComponetData(CallBack);
	}
	if (UIBase != nullptr)
	{
		UIBase->BindGetComponetData(CallBack);
	}
}

void UMESComponetData::UnBindGetComponetData(FUpdateDataSingleDelegate CallBack)
{
	if (actorBase != nullptr)
	{
		actorBase->UnBindGetComponetData(CallBack);
	}
	if (UIBase != nullptr)
	{
		UIBase->UnBindGetComponetData(CallBack);
	}
}

void UMESComponetData::SetFinish()
{
	SaveDataToComponet();
	if (actorBase != nullptr)
	{
		AMESPointComponetBase* p = Cast<AMESPointComponetBase>(actorBase);
		if (p != nullptr)
		{
			p->SetFinishForPoint();
		}
		else
		{
			actorBase->SetFinishForModel();
		}
	}
	if (UIBase != nullptr)
	{
		UIBase->SetFinish();
	}
}

void UMESComponetData::SaveDataToComponet()
{
	if (actorBase != nullptr)
	{
		actorBase->SetMESComponetData(this);
	}
	if (UIBase != nullptr)
	{
		UIBase->SetMESComponetData(this);
	}
}

void UMESComponetData::FetechDataTable()
{
	if (actorBase != nullptr)
	{
		TableData = actorBase->TableData;
	}
	if (UIBase != nullptr)
	{
		TableData = UIBase->TableData;
	}
}
