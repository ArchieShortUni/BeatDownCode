// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NoteEnemy.h"
#include "GameFramework/Actor.h"
#include "ScoreHUD.h"
#include "RythmButton.h"
#include "ScoreManager.generated.h"

UCLASS()
class RYTHMEPROTOTYPE_API AScoreManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AScoreManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	int CurrentStreak;

	int CurrentMultiBand;
	std::vector<int> MultiplyerBands = {5,15,30,50,10000};
	std::vector<int> NoteTypeScores ={0,100,150,200}; 

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void LinkToButtons();

	void AddNoteDelegate(ANoteEnemy* Note); 

	UPROPERTY(EditAnywhere)
	int BaseHitScore; 
	
	UPROPERTY(BlueprintReadWrite)
	int TotalScore;

	UPROPERTY(BlueprintReadWrite)
	int CurrentComboScore;
	
	UPROPERTY(BlueprintReadWrite)
	int CurrentMultiplyer;

	UFUNCTION()
	void Hit(float AccuracyOfHit,float NoteType,FVector HitLocation);

	UFUNCTION()
	void Miss(float NoteType);

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UScoreHUD> ScoreHUDClass;

	UPROPERTY()
	class UScoreHUD* ScoreHUD;
	
	

};
