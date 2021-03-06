// Fill out your copyright notice in the Description page of Project Settings.

#include "MyGame.h"
#include "MyProjectile.h"
#include "MyTarget.h"
#include "Containers/UnrealString.h"
#include "MyGameCharacter.h"
#include "MyGameGameMode.h"
#include "MyGameInstance.h"
#include "Engine.h"


// Sets default values
AMyProjectile::AMyProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!RootComponent)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSceneComponent"));
	}
	if (!CollisionComponent)
	{
		// Use a sphere as a simple collision representation.
		CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
		// Set the sphere's collision profile name to "Projectile".
		CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
		// Event called when component hits something.
		CollisionComponent->OnComponentHit.AddDynamic(this, &AMyProjectile::OnHit);
		// Set the sphere's collision radius.
		CollisionComponent->InitSphereRadius(15.0f);
		// Set the root component to be the collision component.
		RootComponent = CollisionComponent;
	}

	if (!ProjectileMovementComponent)
	{
		// Use this component to drive this projectile's movement.
		ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
		ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
		ProjectileMovementComponent->InitialSpeed = 3000.0f;
		//ProjectileMovementComponent->InitialSpeed = 500.0f;
		ProjectileMovementComponent->MaxSpeed = 3000.0f;
		ProjectileMovementComponent->bRotationFollowsVelocity = true;
		ProjectileMovementComponent->bShouldBounce = true;
		ProjectileMovementComponent->Bounciness = 0.3f;
		ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	}

	if (!ProjectileMeshComponent)
	{
		ProjectileMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshComponent"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh(TEXT("'/Game/ThirdPerson/Meshes/Sphere.Sphere'"));
		if (Mesh.Succeeded())
		{
			ProjectileMeshComponent->SetStaticMesh(Mesh.Object);
		}
	}

	static ConstructorHelpers::FObjectFinder<UMaterial>Material(TEXT("'/Game/ThirdPerson/Meshes/SphereMaterial.SphereMaterial'"));
	if (Material.Succeeded())
	{
		ProjectileMaterialInstance = UMaterialInstanceDynamic::Create(Material.Object, ProjectileMeshComponent);
	}
	ProjectileMeshComponent->SetMaterial(0, ProjectileMaterialInstance);
	ProjectileMeshComponent->SetRelativeScale3D(FVector(0.09f, 0.09f, 0.09f));
	ProjectileMeshComponent->SetupAttachment(RootComponent);

	// Delete the projectile after 3 seconds.
	InitialLifeSpan = 3.0f;

	

}



// Initializes the projectile's velocity in the shoot direction.
void AMyProjectile::FireInDirection(const FVector& ShootDirection)
{
	ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;
}

// Called when the projectile hits something.
void AMyProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor != this && OtherComponent->IsSimulatingPhysics())
	{
		OtherComponent->AddImpulseAtLocation(ProjectileMovementComponent->Velocity * 100.0f, Hit.ImpactPoint);

		if (OtherActor->IsA(AMyTarget::StaticClass())) {
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, TEXT("TARGET HIT"));
			bHit = true;
			AMyTarget* TargetHit = Cast<AMyTarget>(OtherActor);
			if (!TargetHit->bWasHit) 
			{
				MyCharacter->TargetsHit++;
				TargetHit->bWasHit = true;

				//total targets from game instance
				UGameInstance* gameInstance = GetGameInstance();
				UMyGameInstance* myGameInstance = Cast<UMyGameInstance>(gameInstance);

				if (myGameInstance->TotalNumberOfTargets == MyCharacter->TargetsHit)
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, TEXT("YOU WON HORAYYY...!!!!"));
					myGameInstance->bGameWon = true;
				}

			}
			else
			{
				//target was already hit
				GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, TEXT("THIS TARGET WAS ALREADY HIT"));
			}
		}
		
	}

	SetLifeSpan(0.05f);
	//Destroy();
}

// Called when the game starts or when spawned
void AMyProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyProjectile::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

