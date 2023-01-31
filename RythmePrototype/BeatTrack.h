// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataTable.h"
#include "CoreMinimal.h"
#include "BeatTrack.generated.h"


USTRUCT()
struct FBeatTrack : public FTableRowBase
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bTop;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bBottom;  
};

