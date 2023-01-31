// Fill out your copyright notice in the Description page of Project Settings.


#include "Conducter.h"
#include "Components/AudioComponent.h"
#include "ConducterDebugHUD.h"
#include "Engine/Note.h"
#include "Kismet/KismetMathLibrary.h"
#include "Windows/LiveCoding/Private/External/LC_HeartBeat.h"


// Sets default values
AConducter::AConducter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AutoReceiveInput = EAutoReceiveInput::Player0;
	PlayerMesh = CreateDefaultSubobject<UStaticMeshComponent>("PlayerMesh"); 
	SongComponent = CreateDefaultSubobject<UAudioComponent>("Song Component"); 
	SongComponent->bAutoDestroy = false;
	ConducterDebugHUDClass = nullptr;
	ConducterDebugHUD = nullptr;

	LeftRythmButton = nullptr;
	RightRythmButton = nullptr;
	TopRythmButton = nullptr;
	BottomRythmButton = nullptr;

	
}


// Called when the game starts or when spawned
void AConducter::BeginPlay()
{
	Super::BeginPlay();


	SecondsPerBeat = 60.f / SongBpm;


	//Setup Test difficulty.
	FDifficultySetting D1 = FDifficultySetting();
	D1.Init(10,95);
	FDifficultySetting D2 = FDifficultySetting();
	D2.Init(20,30);
	FDifficultySetting D3 = FDifficultySetting();
	D3.Init(120,70);
	FDifficultySetting D4 = FDifficultySetting();
	D4.Init(180,50);
	FDifficultySetting D5 = FDifficultySetting();
	D5.Init(220,70);
	FDifficultySetting D6 = FDifficultySetting();
	D6.Init(270,20);
	FDifficultySetting D7 = FDifficultySetting();
	D7.Init(300,95);

	LevelDifficulties.Add(D1);
	LevelDifficulties.Add(D2);
	LevelDifficulties.Add(D3);
	LevelDifficulties.Add(D4);
	LevelDifficulties.Add(D5);
	LevelDifficulties.Add(D6);
	LevelDifficulties.Add(D7);

	CurrentDifficulty = LevelDifficulties[0];
	//Setup The Beat Array
	int Count = 0; 
	for(auto i : SongBeatTrack->GetRowMap())
	{
		FBeatTrack* RowData = (FBeatTrack*)(i.Value);
		UE_LOG(LogTemp,Warning,TEXT("Beat: %d"),Count);

		BeatTrackArray.Emplace(*RowData); 
		Count++; 
	}
	UE_LOG(LogTemp,Warning,TEXT("Beats In Track: %d"),BeatTrackArray.Num());
	NextBeatTrack = BeatTrackArray[0];
	TotalBeats = BeatTrackArray.Num(); 


	//Setup Song
	
	SongComponent->SetSound(Song);
	SongDuration = Song->Duration; 
	if (GetWorld())
	{
		StartTimeSeconds = UGameplayStatics::GetRealTimeSeconds(GetWorld());
	}
	SongComponent->Play();


	//Spawn debug UI

	if(bShowDebug)
	{
		if(ConducterDebugHUDClass)
		{
			ConducterDebugHUD = Cast<UConducterDebugHUD>(CreateWidget(GetWorld(),ConducterDebugHUDClass)); 
			ConducterDebugHUD->AddToPlayerScreen(); 
		}
	}


	//Spawn Rythm Buttons
	
	FVector PlayerPosition = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
	FRotator PlayerRotation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorRotation();

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this; 
	float DistanceFromPlayer = 150.f; 
	LeftRythmButton = GetWorld()->SpawnActor<ARythmButton>(RythmButtonClassLeft,PlayerPosition+FVector(.0f,-DistanceFromPlayer,0.f),PlayerRotation,SpawnParameters); 
	RightRythmButton = GetWorld()->SpawnActor<ARythmButton>(RythmButtonClassRight,PlayerPosition+FVector(.0f,DistanceFromPlayer,0.f),PlayerRotation+FRotator(0.f,180.f,0.f),SpawnParameters); 

	TopRythmButton = GetWorld()->SpawnActor<ARythmButton>(RythmButtonClassTop,PlayerPosition+FVector(DistanceFromPlayer,0.f,0.f),PlayerRotation+FRotator(0.f,90.f,0.f),SpawnParameters); 
	BottomRythmButton = GetWorld()->SpawnActor<ARythmButton>(RythmButtonClassBottom,PlayerPosition+FVector(-DistanceFromPlayer,0.f,0.f),PlayerRotation+FRotator(0.f,-90.f,0.f),SpawnParameters); 

	LeftRythmButton->ButtonDirection = DIR_LEFT;
	RightRythmButton->ButtonDirection = DIR_RIGHT;
	TopRythmButton->ButtonDirection = DIR_UP;
	BottomRythmButton->ButtonDirection = DIR_DOWN;

	LeftRythmButton->DirectionVector = FVector(.0f,-1.f,0.f);
	RightRythmButton->DirectionVector = FVector(.0f,1.f,0.f);
	TopRythmButton->DirectionVector = FVector(1.0f,0.f,0.f);
	BottomRythmButton->DirectionVector = FVector(-1.0f,0.f,0.f);


	OnBeatDelegate.AddDynamic(LeftRythmButton, &ARythmButton::NextBeat);
	OnBeatDelegate.AddDynamic(RightRythmButton, &ARythmButton::NextBeat);
	OnBeatDelegate.AddDynamic(TopRythmButton, &ARythmButton::NextBeat);
	OnBeatDelegate.AddDynamic(BottomRythmButton, &ARythmButton::NextBeat);
	
	LeftRythmButton->OnEnemyHitDelegate.AddDynamic(this,&AConducter::Hit);
	RightRythmButton->OnEnemyHitDelegate.AddDynamic(this,&AConducter::Hit);
	TopRythmButton->OnEnemyHitDelegate.AddDynamic(this,&AConducter::Hit);
	BottomRythmButton->OnEnemyHitDelegate.AddDynamic(this,&AConducter::Hit);

	//Link Score manager to buttons
	TArray<AActor*> ScoreManagers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),AScoreManager::StaticClass(),ScoreManagers);
	ScoreManager = Cast<AScoreManager>(ScoreManagers[0]);
	ScoreManager->LinkToButtons(); 
}

void AConducter::Beat()
{
	//Play Current Beat
	
	int EnemyTypeRand = FMath::RandRange(0,100);
	int EnemyType = 1; 
	if(EnemyTypeRand > CurrentDifficulty.NoteType2Chance)
	{
		EnemyType = 2; 
	}
	
	if(NextBeatTrack.bBottom)
	{
		SpawnNoteEnemy(BottomRythmButton,EnemyType,DIR_DOWN); 
	}
	if(NextBeatTrack.bTop)
	{
		SpawnNoteEnemy(TopRythmButton,EnemyType,DIR_UP); 
	}
	if(NextBeatTrack.bLeft){
		SpawnNoteEnemy(LeftRythmButton,EnemyType,DIR_LEFT); 
	}
	if(NextBeatTrack.bRight)
	{
		SpawnNoteEnemy(RightRythmButton,EnemyType,DIR_RIGHT); 
	}
	//Load in the next beat

	if(NextBeat <= BeatTrackArray.Num())
	{
		NextBeatTrack = BeatTrackArray[static_cast<int>(NextBeat)];
	}

}

void AConducter::SpawnNoteEnemy(ARythmButton* RythmButton, int EnemyType, ERythmDirections ButtonDirection)
{
	const FVector PlayerPosition = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
	const FRotator PlayerRotation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorRotation();
	const FVector TargetLocation = RythmButton->GetLocationAtBeat(10);
	const FVector Direction = TargetLocation - PlayerPosition;
	FVector SpawnPosition; 

	//Calculate Spawn Position
	//If regular type spawn as normal
	if(EnemyType == 1){
		 SpawnPosition = PlayerPosition + (Direction * 10.f); }
	else{
		//If type 2 spawn at a different location
		//Generate angle between 0 an 180, - 90 to get + or -
		float angle = FMath::FRandRange(0.f,3.14159/2) - (3.14159/4);
		if(angle <= 0){angle = (3.14159*2) - abs(angle); }
		
		float s = sin(abs(angle));
		float c = cos(abs(angle));
		float xnew = (Direction.X * c) - (Direction.Y * s);
		float ynew = (Direction.X * s) + (Direction.Y * c);
		
		 SpawnPosition = PlayerPosition + (FVector(xnew,ynew,0.f) * 10.f);
	}
	const FTransform NoteTransform = FTransform(PlayerRotation, SpawnPosition,FVector(1.0f, 1.0f, 1.0f));


	//Get note that comes before it and attach this note as the following

	//Spawn NoteEnemy
	ANoteEnemy* SpawnedNote = GetWorld()->SpawnActorDeferred<ANoteEnemy>(NoteEnemyClass, NoteTransform,this);
	//Add to the active notes
	ActiveNotesArray.Add(SpawnedNote);
	//Create Material Instance 
	UMaterialInstanceDynamic* EnemyDynamicMaterial = UMaterialInstanceDynamic::Create(EnemyMaterialBase, NULL); 
	EnemyDynamicMaterial->SetVectorParameterValue(TEXT("ButtonColour"),RythmButton->GetDiffuseColour());

	UMaterialInstanceDynamic* DissolveMaterialMat = UMaterialInstanceDynamic::Create(DissolveMaterialBase, NULL);
	DissolveMaterialMat->SetVectorParameterValue(TEXT("Base Colour"),RythmButton->GetDiffuseColour());
	DissolveMaterialMat->SetVectorParameterValue(TEXT("Glow Colour"),RythmButton->GetDiffuseColour());


	//Set Note Variables 	
	SpawnedNote->CurrentSongTime = SongDuration * CurrentPercentage; 
	SpawnedNote->EnemyMat = EnemyDynamicMaterial;
	SpawnedNote->ButtonDirection = ButtonDirection;
	SpawnedNote->DissolveMaterialMat = DissolveMaterialMat;
	SpawnedNote->NoteType = EnemyType; 
	const float ProjectedHit = (SongDuration * CurrentPercentage) +( SecondsPerBeat * 10);
	
	SpawnedNote->SetTarget(NoteTransform.GetLocation(),TargetLocation,ProjectedHit);
	SpawnedNote->FinishSpawning(NoteTransform);

	RythmButton->AddNote(SpawnedNote);

	
	//Setup Delegates 
	ScoreManager->AddNoteDelegate(SpawnedNote);
	SpawnedNote->OnMissDelegate.AddDynamic(this,&AConducter::Miss); 

}

void AConducter::Miss(float NoteType)
{
	if(!(NoteType == -1))
	{
		//Get base damage
		float Damage = 0;
		if(NoteType == 1){Damage = 5;}
		else if (NoteType == 2){Damage = 4;}
		//times by current score multiplier
	
		PlayerHealth -= Damage;
	
		if(PlayerHealth <= 0 )
		{
			PlayerHealth = 0; 
		}
	}
}

void AConducter::Hit(float AccuracyOfHit, float NoteType, FVector HitLocation)
{
	
	FVector Forward = HitLocation - GetActorLocation();
	FRotator Rot = UKismetMathLibrary::MakeRotFromXZ(Forward, FVector(0.f,0.f,1.0f));
	Rot.Yaw -= 90; 
	SetActorRotation(Rot);
}


// Called every frame
void AConducter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	TimeSinceStarted = UGameplayStatics::GetTimeSeconds(GetWorld()) - StartTimeSeconds;

	
	SongPositionInBeats   = (SongDuration * CurrentPercentage  ) / SecondsPerBeat;
	
	//Duration Calculated Percentage
	CurrentDurationPercentage  = (1 / Song->Duration) * TimeSinceStarted;
	
	//Difference Between two percentages
	PercentageDifference = abs(CurrentPercentage - CurrentDurationPercentage); 

	//Update all current notes Time to sync with the conducter
	ST = (SongDuration * CurrentPercentage)- StartingBeatOffset; 
	for(ANoteEnemy* note: ActiveNotesArray)
	{
		if(IsValid(note))
		{
			note->CurrentSongTime = ST;
		}
	}
	LeftRythmButton->CurrentSongTime = ST;
	RightRythmButton->CurrentSongTime = ST;
	TopRythmButton->CurrentSongTime = ST;
	BottomRythmButton->CurrentSongTime = ST;

	LeftRythmButton->CurrentBeat = SongPositionInBeats;
	RightRythmButton->CurrentBeat = SongPositionInBeats;
	TopRythmButton->CurrentBeat = SongPositionInBeats;
	BottomRythmButton->CurrentBeat = SongPositionInBeats;
	
	//Next Beat
	if(SongPositionInBeats >= NextBeat)
	{
		//Beat function

		//Check Difficulty
		for(FDifficultySetting FD: LevelDifficulties)
		{
			if(FD.BeatOfDifficultyChange == NextBeat)
			{
				CurrentDifficulty = FD; 
			}
		}
		
		OnBeatDelegate.Broadcast(NextBeat);
		NextBeat++;
		GEngine->AddOnScreenDebugMessage(-1, .4f, FColor::Yellow, TEXT("Beat!"));	
		Beat(); 
	}
	if(bShowDebug)
	{
		ConducterDebugHUD->TickUpdate(CurrentDurationPercentage, CurrentPercentage,PercentageDifference,TimeSinceStarted,PlayerHealth);
	}
}

void AConducter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction<FInputPressedDelegate>("Left",IE_Pressed, this, &AConducter::InputPressed,ERythmDirections::DIR_LEFT);
	PlayerInputComponent->BindAction<FInputPressedDelegate>("Left",IE_Released, this, &AConducter::InputReleased,ERythmDirections::DIR_LEFT);

	PlayerInputComponent->BindAction<FInputPressedDelegate>("Up",IE_Pressed, this, &AConducter::InputPressed,ERythmDirections::DIR_UP);
	PlayerInputComponent->BindAction<FInputPressedDelegate>("Up",IE_Released, this, &AConducter::InputReleased,ERythmDirections::DIR_UP);


	PlayerInputComponent->BindAction<FInputPressedDelegate>("Down",IE_Pressed, this, &AConducter::InputPressed,ERythmDirections::DIR_DOWN);
	PlayerInputComponent->BindAction<FInputPressedDelegate>("Down",IE_Released, this, &AConducter::InputReleased,ERythmDirections::DIR_DOWN);


	PlayerInputComponent->BindAction<FInputPressedDelegate>("Right",IE_Pressed, this, &AConducter::InputPressed,ERythmDirections::DIR_RIGHT);
	PlayerInputComponent->BindAction<FInputPressedDelegate>("Right",IE_Released, this, &AConducter::InputReleased,ERythmDirections::DIR_RIGHT);


}

void AConducter::InputPressed(ERythmDirections Direction)
{
	switch(Direction)
	{
	case DIR_LEFT:
		LeftRythmButton->Activate();
		break;
		
	case DIR_UP:
		TopRythmButton->Activate();
		break;

	case DIR_RIGHT:
		RightRythmButton->Activate();
		break;
		
	case DIR_DOWN:
		BottomRythmButton->Activate(); 
		break; 
	default: ;
	}

	
}

void AConducter::InputReleased(ERythmDirections Direction)
{

	switch(Direction)
	{
	case DIR_LEFT:
		LeftRythmButton->Deactivate();
		break;
		
	case DIR_UP:
		TopRythmButton->Deactivate();
		break;

	case DIR_RIGHT:
		RightRythmButton->Deactivate();
		break;
		
	case DIR_DOWN:
		BottomRythmButton->Deactivate(); 
		break; 
	default: ;
	}
	
}
