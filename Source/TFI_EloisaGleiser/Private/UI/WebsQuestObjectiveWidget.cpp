#include "UI/WebsQuestObjectiveWidget.h"
#include "Components/TextBlock.h"

void UWebsQuestObjectiveWidget::UpdateObjective(
	const FWebsQuestObjective& Objective)
{
	const bool bCompleted =Objective.CurrentAmount >= Objective.RequiredAmount;

	// Estado
	if (TXT_Status)
	{
		TXT_Status->SetText(FText::FromString(bCompleted ? TEXT("[X]") : TEXT("[ ]")));
	}

	// Nombre del objetivo
	if (TXT_ObjectiveName)
	{
		TXT_ObjectiveName->SetText(FText::FromName(Objective.ObjectiveID));
	}

	// Progreso
	if (TXT_Progress)
	{
		TXT_Progress->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"),Objective.CurrentAmount,Objective.RequiredAmount)));
	}
	
}