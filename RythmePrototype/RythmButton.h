 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/BoxComponent.h"
#include "RythmDirection.h"
#include "NoteEnemy.h"
#include "RythmButton.generated.h"



USTRUCT()
struct FButtonMovement
{
	GENERATED_BODY()
public:
	UPROPERTY()
	float BeatOfAction;

	UPROPERTY()
	float DistanceFromPlayer;

	void Init(float Beat, float Distance)
	{
		BeatOfAction = Beat;
		DistanceFromPlayer = Distance; 
	}
	
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnEnemyHitSignature, float, TimeToTarget, float,NoteType,FVector,HitLocation); 

UCLASS()
class RYTHMEPROTOTYPE_API ARythmButton : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ARythmButton();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category="Visuals")
	UStaticMeshComponent* ButtonMesh;

	UPROPERTY(EditAnywhere, Category="Visuals")
	UMaterial* DiffusedMaterial;

	UPROPERTY(EditAnywhere, Category="Visuals")
	FColor DiffusedColour;
	
	UPROPERTY(EditAnywhere, Category="Visuals")
	UMaterialInstance* ActivatedMaterial;

	UPROPERTY(EditAnywhere, Category="Logic")
	UBoxComponent* ActivatedHitbox;

	UPROPERTY(EditAnywhere)
	USoundBase* HitSoundBase;
	
	UPROPERTY(VisibleAnywhere) 
	UAudioComponent* HitSound;

	UPROPERTY(VisibleAnywhere) 
	bool bHitNote;

	UPROPERTY(VisibleAnywhere)
	FVector SpawnPosition;
	
	TArray<FButtonMovement> ButtonMovementArray;

	UPROPERTY(VisibleAnywhere)
	int CurrentButtonMovementArrayPos; 

	TQueue<ANoteEnemy*> DirectedNotesQueue; 


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	void Activate();

	void Deactivate();

	void AddNote(ANoteEnemy* SpawnedNote);

	UFUNCTION()
	void Miss(float NoteType);

	UFUNCTION()
	void NextBeat(int BeatCount); 

	FVector GetLocationAtBeat(float BeatsAhead);
	
	FColor GetDiffuseColour(){return DiffusedColour;}
	
	UPROPERTY(VisibleAnywhere)
	FVector DirectionVector;
	
	UPROPERTY(VisibleAnywhere)
	float CurrentSongTime;

	UPROPERTY(VisibleAnywhere)
	float CurrentBeat = 0;

	UPROPERTY(BlueprintAssignable)
	FOnEnemyHitSignature OnEnemyHitDelegate;

	UPROPERTY(VisibleAnywhere)
	TEnumAsByte<ERythmDirections> ButtonDirection;

	UPROPERTY(BlueprintAssignable)
	FOnMissSignature OnMissDelegate;
	

protected:
	UPROPERTY(VisibleAnywhere)
	float TimeSinceActivated;

	UPROPERTY(VisibleAnywhere)
	float TimeInputStarted;

	UFUNCTION()
	void OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere)
	bool bIsActive; 
};

