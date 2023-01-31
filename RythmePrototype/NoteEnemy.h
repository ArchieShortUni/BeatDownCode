// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "RythmDirection.h"
#include "Components/SkeletalMeshComponent.h"
#include "NoteEnemy.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMissSignature,float, NoteType);

UCLASS()
class RYTHMEPROTOTYPE_API ANoteEnemy : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ANoteEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	//UPROPERTY(EditAnywhere, Category="Visuals")
	//UStaticMeshComponent* NoteMesh;

	
	UPROPERTY(BlueprintReadWrite,EditAnywhere, Category="Visuals")
	USkeletalMeshComponent* NoteSkeleton;

	
	UPROPERTY(EditAnywhere, Category="Visuals")
	UMaterial* TestMat;

	UPROPERTY(BlueprintReadOnly)
	float DeathTimer;

	UPROPERTY()
	float DissolveTimer;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	void SetTarget(FVector SpawnPosition, FVector playerHitPoint, float TimeToHitTarget);
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void KillNote();

	bool IsAlive(){return bAlive;}

	UPROPERTY(BlueprintReadWrite,VisibleAnywhere)
	float CurrentSongTime;
	
	UPROPERTY(BlueprintReadWrite,VisibleAnywhere)
	float HitSongTime;

	UPROPERTY(VisibleAnywhere)
	float NoteType;


	UPROPERTY(VisibleAnywhere)
	UMaterialInstanceDynamic* EnemyMat;
	
	UPROPERTY(VisibleAnywhere)
	UMaterialInstanceDynamic* DissolveMaterialMat;
	
	UPROPERTY(VisibleAnywhere)
	TEnumAsByte<ERythmDirections> ButtonDirection; 

	float GetTimeToTarget() const;

	UPROPERTY(BlueprintAssignable)
	FOnMissSignature OnMissDelegate;
	
	
private:
	bool bAlive = true; 

	float NoteSpeed;
	FVector Direction;
	
	FVector StartPoint;
	FVector HitTargetPoint; 
	
	float StartSongTime;

};
