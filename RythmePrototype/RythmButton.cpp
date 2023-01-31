// Fill out your copyright notice in the Description page of Project Settings.


#include "RythmButton.h"
#include "Conducter.h"
#include "NoteEnemy.h"
#include "Components/AudioComponent.h"
#include "Engine/Note.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ARythmButton::ARythmButton()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>("Button Mesh");
	ButtonMesh->SetMaterial(0,DiffusedMaterial); 
	RootComponent = ButtonMesh;


	ActivatedHitbox = CreateDefaultSubobject<UBoxComponent>("HitBox");
	ActivatedHitbox->SetupAttachment(ButtonMesh);
	ActivatedHitbox->SetGenerateOverlapEvents(true);
	ActivatedHitbox->ShapeColor = FColor(0.030858,0.588706,1.0);
	ActivatedHitbox->SetHiddenInGame(true); 

	ActivatedHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	//Activated Colour = FColor(0.030858,0.588706,1.0);

	//TEST SETUP

	int MovementTest = FMath::RandRange(1,2);
	
	FButtonMovement M1 = FButtonMovement();
	FButtonMovement M2 = FButtonMovement();
	FButtonMovement M3 = FButtonMovement();
	FButtonMovement M4 = FButtonMovement();
	FButtonMovement M5 = FButtonMovement();
	FButtonMovement M6 = FButtonMovement();
	FButtonMovement M7 = FButtonMovement();
	FButtonMovement M8 = FButtonMovement();
	FButtonMovement M9 = FButtonMovement();
	FButtonMovement M10 = FButtonMovement();
	FButtonMovement M11= FButtonMovement();

	if(MovementTest == 1)
	{
		M1.Init(10.f,100.f);
		M2.Init(20.f,200.f);
		M3.Init(30.f,100.f);
		M4.Init(40.f,0.f);
		M5.Init(70.f,100.f);
		M6.Init(100.f,200.f);
		M7.Init(130.f,100.f);
		M8.Init(170.f,0.f);
		M9.Init(200.f,200.f);
		M10.Init(250.f,100.f);
		M11.Init(270.f,0.f);
	}
	else{
		M1.Init(10.f,100.f);
		M2.Init(20.f,159.f);
		M3.Init(30.f,100.f);
		M4.Init(40.f,30.f);
		M5.Init(70.f,100.f);
		M6.Init(100.f,160.f);
		M7.Init(140.f,100.f);
		M8.Init(180.f,0.f);
		M9.Init(210.f,200.f);
		M10.Init(230.f,100.f);
		M11.Init(280.f,0.f);
		}
	

	ButtonMovementArray.Add(M1);
	ButtonMovementArray.Add(M2);
	ButtonMovementArray.Add(M3);
	ButtonMovementArray.Add(M4);
	ButtonMovementArray.Add(M5);
	ButtonMovementArray.Add(M6);
	ButtonMovementArray.Add(M7);
	ButtonMovementArray.Add(M8);
	ButtonMovementArray.Add(M9);
	ButtonMovementArray.Add(M10);
	ButtonMovementArray.Add(M11);
	
	HitSoundBase = CreateDefaultSubobject<USoundBase>("Gunshot Base sound");

	HitSound = CreateDefaultSubobject<UAudioComponent>("Gunshot Component"); 
	HitSound->bAutoDestroy = false;

	SpawnPosition = FVector(0.f,0.f,0.f);
	CurrentButtonMovementArrayPos = -1; 
}
 
// Called when the game starts or when spawned
void ARythmButton::BeginPlay()
{
	Super::BeginPlay();
	ActivatedHitbox->OnComponentBeginOverlap.AddDynamic(this,&ARythmButton::OverlapBegin);
	HitSound->SetSound(HitSoundBase);

	SpawnPosition = GetActorLocation(); 

}

// Called every frame
void ARythmButton::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//Check to see if it has been active too long

	if(TimeSinceActivated + .5f < CurrentSongTime)
	{
		Deactivate(); 
	}
	if(!DirectedNotesQueue.IsEmpty())
	{
		ANoteEnemy* TempNote; 
		DirectedNotesQueue.Peek(TempNote);
		
		const FVector NoteLocation = TempNote->GetActorLocation();
		if(FVector::Distance(NoteLocation,GetActorLocation()) > 40.f)
		{
			//const FRotator ButtonRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(),NoteLocation);
			//ButtonMesh->SetWorldRotation(ButtonRotation);

			FVector Forward = NoteLocation - GetActorLocation();
			FRotator Rot = UKismetMathLibrary::MakeRotFromXZ(Forward, FVector(0.f,0.f,1.0f));
			Rot.Yaw += 90; 
			SetActorRotation(Rot);
			
		}
	}

	//Calculate Movement
	// if empty do nothing

// if only 1 stay at that location
	if(!ButtonMovementArray.IsEmpty())
	{
		if(ButtonMovementArray.Num() ==1)
		{
			SetActorLocation(GetActorLocation() + (DirectionVector * FVector(ButtonMovementArray[0].DistanceFromPlayer))); 
		}
		else
		{
			//Starting point 
			if(CurrentButtonMovementArrayPos == -1)
			{

				//Calculate difference
				float Distance = ButtonMovementArray[0].DistanceFromPlayer * ((1 / ButtonMovementArray[0].BeatOfAction) * CurrentBeat);
				SetActorLocation(SpawnPosition + (DirectionVector * FVector(Distance)));

				if(CurrentBeat > ButtonMovementArray[CurrentButtonMovementArrayPos+1].BeatOfAction)
				{
					CurrentButtonMovementArrayPos ++; 
				}
			}
			else if(CurrentButtonMovementArrayPos != ButtonMovementArray.Num() -1)
			{
				//Difference between two
				float Difference =   ButtonMovementArray[CurrentButtonMovementArrayPos+1].DistanceFromPlayer - ButtonMovementArray[CurrentButtonMovementArrayPos].DistanceFromPlayer;
				float BeatDifference = ButtonMovementArray[CurrentButtonMovementArrayPos+1].BeatOfAction - ButtonMovementArray[CurrentButtonMovementArrayPos].BeatOfAction;
				float Distance = ButtonMovementArray[CurrentButtonMovementArrayPos].DistanceFromPlayer + (Difference * ((1 / BeatDifference) * (CurrentBeat-ButtonMovementArray[CurrentButtonMovementArrayPos].BeatOfAction)));
				SetActorLocation(SpawnPosition + (DirectionVector * FVector(Distance)));

				if(CurrentBeat > ButtonMovementArray[CurrentButtonMovementArrayPos+1].BeatOfAction)
				{
					CurrentButtonMovementArrayPos ++; 
				}
			}
			

		}
	}


	
}

// Called to bind functionality to input
void ARythmButton::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ARythmButton::Activate()
{
	ButtonMesh->SetMaterial(0,ActivatedMaterial);
	bHitNote = false; 
	ActivatedHitbox->SetHiddenInGame(false); 
	ActivatedHitbox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	
	bIsActive = true;
	
	TimeSinceActivated = CurrentSongTime;
	
}

void ARythmButton::Deactivate()
{
	if(bIsActive)
	{
		ButtonMesh->SetMaterial(0,DiffusedMaterial);
	
		ActivatedHitbox->SetHiddenInGame(true);
		ActivatedHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		if(!bHitNote)
		{
			//Miss
			OnMissDelegate.Broadcast(-1);

		}
		//Debug
		bIsActive = false;
		TimeSinceActivated = CurrentSongTime;
	} 
}

void ARythmButton::AddNote(ANoteEnemy* SpawnedNote)
{
	SpawnedNote->OnMissDelegate.AddDynamic(this,&ARythmButton::Miss); 
	DirectedNotesQueue.Enqueue(SpawnedNote);
}

void ARythmButton::Miss(float NoteType)
{
	DirectedNotesQueue.Pop(); 
}

void ARythmButton::NextBeat(int BeatCount)
{
	CurrentBeat = BeatCount;
	UE_LOG(LogTemp,Warning,TEXT("Beat Number:  %d"),BeatCount)
}

FVector ARythmButton::GetLocationAtBeat(float BeatsAhead)
{
	float beat = CurrentBeat + BeatsAhead; 
	//If it is empty or has a size of 1 return the current position
	if(ButtonMovementArray.IsEmpty())
	{
		return SpawnPosition; 
	}

	if(ButtonMovementArray.Num() == 1)
	{
		return SpawnPosition + (DirectionVector * FVector(ButtonMovementArray[0].DistanceFromPlayer)); 
	}
	
	//main chunk of code, Get the last beat and the one before, get the difference between them at the percentage based on the beat.
	int i = 0; 
	for(FButtonMovement Movement: ButtonMovementArray)
	{
		if(Movement.BeatOfAction > beat)
		{
			//Found higher beat execute code and return
			
			float Difference =   Movement.DistanceFromPlayer - ButtonMovementArray[i-1].DistanceFromPlayer;
			float BeatDifference = Movement.BeatOfAction - ButtonMovementArray[i-1].BeatOfAction;
			float Distance = ButtonMovementArray[i-1].DistanceFromPlayer + (Difference * ((1 / BeatDifference) * (beat-ButtonMovementArray[i-1].BeatOfAction)));
			return (SpawnPosition + (DirectionVector * FVector(Distance)));
		}
		i++; 
	}
	return SpawnPosition + (DirectionVector * FVector(ButtonMovementArray.Last().DistanceFromPlayer)); 

	
}


void ARythmButton::OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ANoteEnemy* HitNote = Cast<ANoteEnemy>(OtherActor);
	if(HitNote && HitNote->ButtonDirection == ButtonDirection && HitNote->IsAlive())
	{
		UE_LOG(LogTemp,Warning,TEXT("OtherActor: %s"),ToCStr(OtherActor->GetClass()->GetName()));
		UE_LOG(LogTemp,Warning,TEXT("Time Since Input: %f"),CurrentSongTime - TimeSinceActivated);
		UE_LOG(LogTemp,Warning,TEXT("Time To Target: %f"),HitNote->GetTimeToTarget());

		if(abs(HitNote->GetTimeToTarget()) <= 0.15f)
		{
			HitSound->Play();
			bHitNote = true; 
			//Hit Correctly, Destroy the note
			OnEnemyHitDelegate.Broadcast(HitNote->GetTimeToTarget(),HitNote->NoteType,HitNote->GetActorLocation());

			HitNote->KillNote();
			//Broadcast the hit 
			DirectedNotesQueue.Pop(); 

		}
	}
}


