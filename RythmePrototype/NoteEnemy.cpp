// Fill out your copyright notice in the Description page of Project Settings.


#include "NoteEnemy.h"
#include "VectorTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
// Sets default values
ANoteEnemy::ANoteEnemy()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;
	//NoteMesh = CreateDefaultSubobject<UStaticMeshComponent>("Button Mesh");
	//RootComponent = NoteMesh; 

	NoteSkeleton = CreateDefaultSubobject<USkeletalMeshComponent>("NoteMesh");
	RootComponent = NoteSkeleton; 

	
	Direction = FVector(0.f,1.f,0.f);
	NoteSpeed = 0.000001f;

	StartPoint = FVector(0.f,0.f,0.f);
	HitTargetPoint = FVector(0.f,0.f,0.f); 
	
	CurrentSongTime = 1;
	HitSongTime = 1;
	StartSongTime = 1;
	
	DeathTimer = 0.f; 
	DissolveTimer = 0.f;
}

// Called when the game starts or when spawned
void ANoteEnemy::BeginPlay()
{
	Super::BeginPlay();
	NoteSkeleton->SetMaterial(1,EnemyMat);
}

// Called every frame
void ANoteEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//Calculate position needed to be in based on start point, end point, start time, desired time, current time.

	if(bAlive)
	{
		float Progress = (1 - (((1/(HitSongTime - StartSongTime))*(HitSongTime - CurrentSongTime))));
		EnemyMat->SetScalarParameterValue(TEXT("Blend"),1 - FMath::Pow(Progress,3)); 
		FVector NewLocation = (HitTargetPoint - StartPoint) * Progress; 
		SetActorLocation(StartPoint +NewLocation);

	
		if(CurrentSongTime > HitSongTime +.35f)
		{
			OnMissDelegate.Broadcast(NoteType);

			//Shoot 
			Destroy();
			//Take Away Health
		}
	}
	else
	{
		DeathTimer += DeltaTime; 
	}
	
	if (DeathTimer > .05f && DeathTimer < .3f)
	{
		CustomTimeDilation = .2f;
		
	}
	else if(DeathTimer > .3f &&  DeathTimer < .7f )
	{
		DissolveTimer+= .5f*DeltaTime; 
		NoteSkeleton->SetPhysicsLinearVelocity(FVector(0.f,0.f,0.f));
		DissolveMaterialMat->SetScalarParameterValue(TEXT("Dissolve"),DissolveTimer);
		

	}
	else if (DeathTimer > .7f)
	{
		Destroy(); 
	}
	
	

}

void ANoteEnemy::SetTarget(FVector SpawnPosition, FVector playerHitPoint,float TimeToHitTarget)
{
	StartSongTime = CurrentSongTime; 
	HitSongTime = TimeToHitTarget; 

	StartPoint = SpawnPosition;
	HitTargetPoint = playerHitPoint;
	
	Direction = UKismetMathLibrary::GetDirectionUnitVector(SpawnPosition , playerHitPoint);
	FVector Difference = SpawnPosition - playerHitPoint;  
	NoteSpeed = TimeToHitTarget / Difference.Size(); 
	
	FVector Forward = HitTargetPoint - GetActorLocation();
	FRotator Rot = UKismetMathLibrary::MakeRotFromXZ(Forward, FVector(0.f,0.f,1.0f));
	Rot.Yaw -= 90; 
	SetActorRotation(Rot);

}

// Called to bind functionality to input
void ANoteEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ANoteEnemy::KillNote()
{
	NoteSkeleton->SetSimulatePhysics(true);
	
	const FVector ForceDirection = GetActorLocation() - GetOwner()->GetActorLocation();
	float angle = FMath::FRandRange(0.f,3.14159/2) - (3.14159/4);
	if(angle <= 0){angle = (3.14159*2) - abs(angle); }
		
	float s = sin(abs(angle));
	float c = cos(abs(angle));
	float xnew = (ForceDirection.X * c) - (ForceDirection.Y * s);
	float ynew = (ForceDirection.X * s) + (ForceDirection.Y * c);
		
	FVector Force = (FVector(xnew,ynew,.1f) * 120.f);
	FString Str = "Spine";
	FName BoneName = FName(*Str); 
	NoteSkeleton->AddImpulse(Force,BoneName);
	NoteSkeleton->SetMaterial(1,DissolveMaterialMat); 
	bAlive = false;
	
}

float ANoteEnemy::GetTimeToTarget() const
{
	return HitSongTime - CurrentSongTime; 
}

