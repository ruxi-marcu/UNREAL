// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "MyGun.generated.h"

//#define TRACE_WEAPON ECC_GameTraceChannel1

class AMyGameCharacter;

USTRUCT()
struct FGunData {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Ammo)
		int32 MaxAmmo;

	UPROPERTY(EditDefaultsOnly, Category = Config)
		float TimeBetweenShots;

	UPROPERTY(EditDefaultsOnly, Category = Ammo)
		int32 ShotCost;

	UPROPERTY(EditDefaultsOnly, Category = Config)
		float GunRange;

	UPROPERTY(EditDefaultsOnly, Category = Config)
		float GunSpread;
};

UCLASS()
class MYGAME_API AMyGun : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyGun();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UFUNCTION()
	void FireFirst(AMyGameCharacter* MyCharacter, UCameraComponent* aimCamera);

	UFUNCTION()
	void FireThird(AMyGameCharacter* MyCharacter);

	UPROPERTY(EditDefaultsOnly, Category = Config)
	FGunData GunConfig;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Collision)
	UBoxComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Config)
	USkeletalMeshComponent* GunMesh;

	// Gun muzzle offset from the camera location.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector MuzzleOffset;

	// Projectile class to spawn.
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class AMyProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = Config)
		USoundCue* FireSound;

	//void SetOwner(AMyGameCharacter *NewOwner);

	void PlayFireSound(USoundCue* sound);

protected:
	AMyGameCharacter *MyCharacter;
	
};
