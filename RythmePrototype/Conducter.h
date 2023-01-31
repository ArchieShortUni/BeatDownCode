// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

#include "RythmButton.h"
#include "ScoreManager.h"
#include "RythmDirection.h" 
#include "NoteEnemy.h"
#include "BeatTrack.h"
#include "Conducter.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBeatSignature, int, BeatCount);

USTRUCT()
struct FDifficultySetting
{
	GENERATED_BODY()
public:
	UPROPERTY()
	float BeatOfDifficultyChange;

	UPROPERTY()
	int NoteType2Chance;

	void Init(float Beat, int NoteT2Ch)
	{
		BeatOfDifficultyChange = Beat;
		NoteType2Chance = NoteT2Ch; 
	}
	
};

UCLASS()
class RYTHMEPROTOTYPE_API AConducter : public APawn
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AConducter();

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bShowDebug = true; 

	//Player Variables
	UPROPERTY(BlueprintReadWrite)
	int PlayerHealth = 100; 


	UPROPERTY()
	TArray<FDifficultySetting> LevelDifficulties;

	UPROPERTY()
	FDifficultySetting CurrentDifficulty; 
	
	//The Song track data 
	UPROPERTY(EditAnywhere)
	UDataTable* SongBeatTrack;

	//Transformed Data converted for easy use
	UPROPERTY(VisibleAnywhere)
	TArray<FBeatTrack> BeatTrackArray;

	UPROPERTY(VisibleAnywhere)
	FBeatTrack NextBeatTrack;
	
	UPROPERTY(EditAnywhere)
	USoundBase* Song; 

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float SongBpm = 118;

	UPROPERTY(BlueprintReadOnly)
	float TotalBeats = 0;

	UPROPERTY(EditAnywhere)
	float StartingBeatOffset = 0.05f;

	UPROPERTY(BlueprintReadOnly)
	float ST = 0;

	
	UPROPERTY(BlueprintReadOnly)
	float SongPositionInBeats = 0; 
//118
	
	UPROPERTY(VisibleAnywhere) 
	UAudioComponent* SongComponent;

	UPROPERTY(BlueprintReadWrite, Category="Song Variables")
	float CurrentPercentage;  


	//Player
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* PlayerMesh; 

	//Rythm Buttons
	UPROPERTY(EditAnywhere)
	TSubclassOf<ARythmButton> RythmButtonClassLeft; 
	UPROPERTY(EditAnywhere)
	TSubclassOf<ARythmButton> RythmButtonClassRight; 
	UPROPERTY(EditAnywhere)
	TSubclassOf<ARythmButton> RythmButtonClassTop; 
	UPROPERTY(EditAnywhere)
	TSubclassOf<ARythmButton> RythmButtonClassBottom; 

	
	UPROPERTY(BlueprintReadWrite)
	ARythmButton* LeftRythmButton;

	
	UPROPERTY(BlueprintReadWrite)
	ARythmButton* RightRythmButton;
	
	UPROPERTY(BlueprintReadWrite)
	ARythmButton* TopRythmButton;

	
	UPROPERTY(BlueprintReadWrite)
	ARythmButton* BottomRythmButton;
	
	//Notes
	UPROPERTY(EditAnywhere)
	UMaterial* EnemyMaterialBase;

	UPROPERTY(EditAnywhere)
	UMaterial* DissolveMaterialBase;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<ANoteEnemy> NoteEnemyClass; 

	UPROPERTY(VisibleAnywhere)
	TArray<ANoteEnemy*> ActiveNotesArray; 
	
	std::vector<ANoteEnemy*> ActiveNotes;

	//Score Manager Reference
	AScoreManager* ScoreManager; 
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Beat();

	void SpawnNoteEnemy(ARythmButton* RythmButton, int EnemyType,ERythmDirections ButtonDirection);

	UFUNCTION() 
	void Miss(float NoteType);

	UFUNCTION()
	void Hit(float AccuracyOfHit,float NoteType,FVector HitLocation);

	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override; 
	
	void OnAudioPlaybackPercent(const USoundWave* PlayingSoundWave, const float PlaybackPercent);


	DECLARE_DELEGATE_OneParam(FInputPressedDelegate, const ERythmDirections);
	
	UFUNCTION(BlueprintCallable,Category="Control")
	void InputPressed(ERythmDirections Direction);

	UFUNCTION(BlueprintCallable,Category="Control")
	void InputReleased(ERythmDirections Direction);
	
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UConducterDebugHUD> ConducterDebugHUDClass;

	UPROPERTY()
	class UConducterDebugHUD* ConducterDebugHUD;

	
	UFUNCTION(BlueprintCallable)
	int GetHealth(){return PlayerHealth; }

	UPROPERTY(BlueprintAssignable)
	FOnBeatSignature OnBeatDelegate;
	
private:
	float StartTimeSeconds;
	float TimeSinceStarted;
	float SongDuration;
	float CurrentDurationPercentage;
	float PercentageDifference;

	//
	float SecondsPerBeat = 0; 
	int NextBeat = 1; 
};




