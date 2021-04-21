// Fill out your copyright notice in the Description page of Project Settings.

#include "MyGame.h"
#include "MyGun.h"
#include "MyProjectile.h"
#include "Engine.h"
#include "Components/ActorComponent.h"



// Sets default values
AMyGun::AMyGun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true; //????

	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComp"));
	RootComponent = CollisionComp;

	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh"));
	GunMesh->AttachTo(RootComponent);

}

void AMyGun::FireFirst(AMyGameCharacter* MyCharacter, UCameraComponent* aimCamera)
{
	// Attempt to fire a projectile.
	if (ProjectileClass)
	{
		PlayFireSound(FireSound);

		FVector CameraLocation = aimCamera->GetComponentLocation();
		FRotator CameraRotation = aimCamera->GetComponentRotation();

		// Set MuzzleOffset to spawn projectiles slightly in front of the camera.
		MuzzleOffset.Set(50.0f, 0.0f, -25.0f);
		
		// Transform MuzzleOffset from camera space to world space.
		FVector MuzzleLocation = CameraLocation + FTransform(CameraRotation).TransformVector(MuzzleOffset);
		FRotator MuzzleRotation = CameraRotation;

		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();

			AMyProjectile* Projectile = World->SpawnActor<AMyProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
			if (Projectile)
			{
				// Set the projectile's initial trajectory.
				FVector LaunchDirection = MuzzleRotation.Vector();

				Projectile->MyCharacter = MyCharacter;
				Projectile->FireInDirection(LaunchDirection);
			}
		}

	
	}

}


void AMyGun::FireThird(AMyGameCharacter* MyCharacter)
{

	// Attempt to fire a projectile.
	if (ProjectileClass)
	{
		PlayFireSound(FireSound);

		FVector GunLocation = GunMesh->GetSocketLocation("socketHold");
		FRotator GunRotation = GunMesh->GetSocketRotation("socketHold");

		// Set MuzzleOffset to spawn projectiles slightly in front of the camera.
		MuzzleOffset.Set(20.0f, 0.0f, 0.0f);

		// Transform MuzzleOffset from camera space to world space.
		FVector MuzzleLocation = GunLocation + FTransform(GunRotation).TransformVector(MuzzleOffset);
		FRotator MuzzleRotation = GunRotation;

		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();

			// Spawn the projectile at the muzzle.
			AMyProjectile* Projectile = World->SpawnActor<AMyProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
			if (Projectile)
			{
				// Set the projectile's initial trajectory.
				FVector LaunchDirection = MuzzleRotation.Vector();

				Projectile->MyCharacter = MyCharacter;
				Projectile->FireInDirection(LaunchDirection);
			}
		}
	}
}

void AMyGun::PlayFireSound(USoundCue* sound)
{
	if (sound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, sound, GetActorLocation());
	}
}

// Called when the game starts or when spawned
void AMyGun::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyGun::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}


