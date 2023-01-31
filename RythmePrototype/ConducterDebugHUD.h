// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ConducterDebugHUD.generated.h" 


/**
 * 
 */
UCLASS(Abstract)

class RYTHMEPROTOTYPE_API UConducterDebugHUD : public UUserWidget
{
	GENERATED_BODY() 

public:
	void TickUpdate(float DurationProgress, float SampleProgress, float Difference, float TimePlaying, int H);

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UTextBlock* DurProg;
	
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UTextBlock* SampProg;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	 UTextBlock* Diff;
	
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UTextBlock* SecsPlaying;

		
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UTextBlock* Health
	;

	

	
	
};
