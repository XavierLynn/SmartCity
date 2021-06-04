// Fill out your copyright notice in the Description page of Project Settings.


#include "MESRadioButton.h"

TMap<uint32, TArray<UMESRadioButton*>> UMESRadioButton::DicButton;

void UMESRadioButton::OnSelect()
{
	IsSelect = true;
	SelectedEvent.Broadcast(IsSelect);
	UPanelWidget* parent = GetParent();
	if (parent != nullptr)
	{
		int32 UUID = parent->GetUniqueID();
		if (DicButton.Contains(UUID))
		{
			TArray<UMESRadioButton*> array = DicButton[UUID];
			for (int i = 0; i < array.Num(); i++)
			{
				if (array[i]->IsSelect == false)
					continue;
				if (array[i] == this)
					continue;
				array[i]->OnUnSelect();
			}
		}
	}
}

void UMESRadioButton::OnUnSelect()
{
	UPanelWidget* parent = GetParent();
	if (parent != nullptr)
	{
		int32 UUID = parent->GetUniqueID();
		if (DicButton.Contains(UUID))
		{
			TArray<UMESRadioButton*> array = DicButton[UUID];
			bool AllHiden = true;
			for (int i = 0; i < array.Num(); i++)
			{
				if (array[i] == this)
					continue;
				if (array[i]->IsSelect)
				{
					AllHiden = false;
				}
			}
			//必须至少要留一个选中状态
			if (AllHiden)
			{
				IsSelect = true;
				return;
			}
		}
	}
	IsSelect = false;
	SelectedEvent.Broadcast(IsSelect);
}

void UMESRadioButton::OnInit()
{
	if (IsSelect)
	{
		OnSelect();
	}
	else
	{
		OnUnSelect();
	}
}

void UMESRadioButton::NativeConstruct()
{
	UPanelWidget* parent = GetParent();
	if (parent != nullptr)
	{
		int32 UUID = parent->GetUniqueID();
		if (!DicButton.Contains(UUID))
		{
			TArray<UMESRadioButton*> array;
			DicButton.Add(UUID, array);
		}
		DicButton[UUID].Add(this);
	}
}

void UMESRadioButton::NativeDestruct()
{
	UPanelWidget* parent = GetParent();
	if (parent != nullptr)
	{
		int32 UUID = parent->GetUniqueID();
		if (DicButton.Contains(UUID))
		{
			DicButton[UUID].Remove(this);
		}

	}
}
