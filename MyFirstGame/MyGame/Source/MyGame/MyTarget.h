// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "MyTarget.generated.h"

UCLASS()
class MYGAME_API AMyTarget : public AActor
{
	GENERATED_BODY()
	
public:	
	AMyTarget();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//called when game ends
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Target mesh
	UPROPERTY(VisibleDefaultsOnly, Category = Target)
	UStaticMeshComponent* TargetMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Target)
	bool bWasHit;

};
