// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class MYGAME_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UMyGameInstance();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Target)
	int TotalNumberOfTargets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Target)
	bool bGameWon;
	
};
