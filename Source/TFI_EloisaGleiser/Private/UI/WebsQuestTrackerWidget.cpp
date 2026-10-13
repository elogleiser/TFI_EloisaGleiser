// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WebsQuestTrackerWidget.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Gameplay/Quests/WebsQuestDefinition.h"
#include "UI/WebsQuestObjectiveWidget.h"
#include "Components/VerticalBox.h"


void UWebsQuestTrackerWidget::ToggleQuestContent()
{
	if (!Border_QuestContent)
	{
		return;
	}

	const bool bIsOpen =Border_QuestContent->GetVisibility() == ESlateVisibility::Visible;

	Border_QuestContent->SetVisibility(bIsOpen ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);

	if (TXT_Arrow)
	{
		TXT_Arrow->SetText(FText::FromString(bIsOpen ? TEXT("▼") : TEXT("▲")));
	}
}

void UWebsQuestTrackerWidget::UpdateQuestDisplay(const FWebsQuestRuntime& QuestData)
{
	if (!QuestData.QuestDefinition)
	{
		return;
	}
	
	const UWebsQuestDefinition* Definition = QuestData.QuestDefinition;

	// Nombre y descripción
	if (TXT_QuestName)
	{
		TXT_QuestName->SetText(Definition->QuestName);
	}

	if (TXT_QuestDescription)
	{
		TXT_QuestDescription->SetText(Definition->Description);
	}
	
	// Objetivos dinámicos
	if (VB_Objectives && ObjectiveWidgetClass)
	{
		VB_Objectives->ClearChildren();

		for (const FWebsQuestObjective& Objective : QuestData.Objectives)
		{
			UWebsQuestObjectiveWidget* ObjectiveWidget =CreateWidget<UWebsQuestObjectiveWidget>(GetOwningPlayer(),ObjectiveWidgetClass);

			if (ObjectiveWidget)
			{
				ObjectiveWidget->UpdateObjective(Objective);

				VB_Objectives->AddChildToVerticalBox(ObjectiveWidget);
			}
		}
	}

	// Recompensa
	if (TXT_Reward)
	{
		TXT_Reward->SetText(FText::FromString(FString::Printf(TEXT("Reward: +%d Silk"),Definition->SilkReward)));
	}
}
