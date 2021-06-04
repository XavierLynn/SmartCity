// Fill out your copyright notice in the Description page of Project Settings.


#include "MessageBox.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Blueprint/WidgetTree.h"

void UMessageBox::Init(FString Message)
{
	UTextBlock* TextMessage = WidgetTree->FindWidget<UTextBlock>("TextBlock_Message");
	if (TextMessage)
	{
		TextMessage->SetText(FText::FromString(Message));
	}

	UButton* ButtonSure = WidgetTree->FindWidget<UButton>("Button_Sure");
	if (ButtonSure)
	{
		ButtonSure->OnClicked.AddDynamic(this, &UMessageBox::OnButtonSure);
	}
}

void UMessageBox::OnButtonSure()
{
	RemoveFromParent();
}
