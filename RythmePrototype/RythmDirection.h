// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RythmDirection.generated.h"

UENUM(BlueprintType)
enum ERythmDirections
{
	DIR_UP	UMETA(DisplayName = "Up"),
	DIR_DOWN	UMETA(DisplayName = "Down"),
	DIR_LEFT	UMETA(DisplayName = "Left"),
	DIR_RIGHT	UMETA(DisplayName = "Right"),
};