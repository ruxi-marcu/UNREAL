// Fill out your copyright notice in the Description page of Project Settings.

#include "MyGame.h"
#include "MyTarget.h"


// Sets default values
AMyTarget::AMyTarget()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!TargetMeshComponent)
	{
		TargetMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TargetMeshComponent"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh(TEXT("'/Game/Geometry/GameObjects/Target/Archery_target.Archery_target'"));
		if (Mesh.Succeeded())
		{
			TargetMeshComponent->SetStaticMesh(Mesh.Object);
		}
	}

}

// Called when the game starts or when spawned
void AMyTarget::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called when the game ends
void AMyTarget::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

}

// Called every frame
void AMyTarget::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}



