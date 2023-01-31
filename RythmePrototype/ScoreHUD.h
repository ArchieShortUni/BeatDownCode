// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "ScoreHUD.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class RYTHMEPROTOTYPE_API UScoreHUD : public UUserWidget
{
public:
	GENERATED_BODY()
	void Update(int TScore, int CCombo, int CMultiplyer);

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UTextBlock* TotalScore;
	
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UTextBlock* CurrentCombo;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UTextBlock* ScoreMultiplyer;
};
