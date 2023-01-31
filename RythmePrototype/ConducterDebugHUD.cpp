// Fill out your copyright notice in the Description page of Project Settings.


#include "ConducterDebugHUD.h"
#include "Components/TextBlock.h"

void UConducterDebugHUD::TickUpdate(float DurationProgress, float SampleProgress, float Difference, float TimePlaying, int H)
{
	DurProg->SetText(FText::FromString(TEXT("Song Progress (Duration): ")+ FString::SanitizeFloat(DurationProgress)));
	
	SampProg->SetText(FText::FromString(TEXT("Song Progress (Samples): ")+ FString::SanitizeFloat(SampleProgress)));

	Diff->SetText(FText::FromString(TEXT("Difference In Progress: ")+ FString::SanitizeFloat(Difference)));

	SecsPlaying->SetText(FText::FromString(TEXT("Time Playing: ")+ FString::SanitizeFloat(TimePlaying)));

	Health->SetText(FText::FromString(TEXT("Player Health: ")+ FString::SanitizeFloat(H)));

}
