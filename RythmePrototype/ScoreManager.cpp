// Fill out your copyright notice in the Description page of Project Settings.


#include "ScoreManager.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
AScoreManager::AScoreManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	TotalScore = 0 ;
	CurrentComboScore = 0;
	CurrentMultiplyer = 1;
	BaseHitScore = 100;  
	CurrentStreak = 0;
	CurrentMultiBand = 0; 
}

// Called when the game starts or when spawned
void AScoreManager::BeginPlay()
{
	Super::BeginPlay();
	
	if(ScoreHUDClass)
	{
		ScoreHUD = Cast<UScoreHUD>(CreateWidget(GetWorld(),ScoreHUDClass)); 
		ScoreHUD->AddToPlayerScreen();
		ScoreHUD->Update(0,0,1); 
	}


}

// Called every frame
void AScoreManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AScoreManager::LinkToButtons()
{
	TArray<AActor*> Buttons;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),ARythmButton::StaticClass(),Buttons);
	for(int i =0;i<Buttons.Num();i++)
	{
		GEngine->AddOnScreenDebugMessage(-1, .4f, FColor::Yellow, TEXT("Button Found!"));	

		ARythmButton* TempButton = Cast<ARythmButton>(Buttons[i]);
		TempButton->OnEnemyHitDelegate.AddDynamic(this,&AScoreManager::Hit);
		TempButton->OnMissDelegate.AddDynamic(this,&AScoreManager::Miss);

	}
}

void AScoreManager::AddNoteDelegate(ANoteEnemy* Note)
{
	Note->OnMissDelegate.AddDynamic(this,&AScoreManager::Miss); 
}

void AScoreManager::Hit(float AccuracyOfHit,float NoteType,FVector HitLocation)
{
	//Check Current combo
	CurrentStreak += 1;
	if(CurrentStreak >= MultiplyerBands[CurrentMultiBand])
	{
		CurrentMultiBand += 1;
		CurrentMultiplyer +=1; 
	}
	
	TotalScore += BaseHitScore * CurrentMultiplyer;
	CurrentComboScore += NoteTypeScores[NoteType] * CurrentMultiplyer;
	
	
	ScoreHUD->Update(TotalScore,CurrentComboScore,CurrentMultiplyer);
	GEngine->AddOnScreenDebugMessage(-1, .4f, FColor::Yellow, TEXT("HIT!"));	

	//Increase score based on bands of accuracy
	
	//Increase multiplyer after x amount of consecutive beats 
}

void AScoreManager::Miss(float NoteType)
{	CurrentStreak = 0;
	CurrentComboScore = 0;
	CurrentMultiplyer = 1;
	CurrentMultiBand = 0; 
	ScoreHUD->Update(TotalScore,CurrentComboScore,CurrentMultiplyer);

	//End current combo and set multiplyer back to 1 
}

