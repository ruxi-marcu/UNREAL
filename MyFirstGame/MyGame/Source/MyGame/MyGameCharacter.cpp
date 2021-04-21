// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "MyGame.h"
#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SceneComponent.h"
#include "Components/CapsuleComponent.h"
#include "MyGameCharacter.h"
#include "MyTarget.h"
#include "MyGun.h"
#include "MyGameInstance.h"

//////////////////////////////////////////////////////////////////////////
// AMyGameCharacter

AMyGameCharacter::AMyGameCharacter()
{
	HasGun = false;
	static ConstructorHelpers::FObjectFinder<UBlueprint> WeaponBlueprint(TEXT("'/Game/ThirdPersonCPP/Blueprints/BP_MyGun.BP_MyGun'"));
	WeaponSpawn = NULL;
	CurrentWeapon = NULL;
	if (WeaponBlueprint.Succeeded()) {
		WeaponSpawn = (UClass*)WeaponBlueprint.Object->GeneratedClass;
	}

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AMyGameCharacter::OnOverlap);
	//GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AMyGameCharacter::OnHitTest);



	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	//CharacterCollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("GunCollisionComp"));
	//CharacterCollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AMyGameCharacter::OnCollision);
	//RootComponent = CharacterCollisionComp;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create a aim camera
	AimCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("AimCamera"));
	AimCamera->SetupAttachment(GetCapsuleComponent());
	AimCamera->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	AimCamera->bUsePawnControlRotation = true;

	// Create a first person mesh component for the owning player.
	FPSMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	check(FPSMesh != nullptr);

	// Only the owning player sees this mesh.
	FPSMesh->SetOnlyOwnerSee(true);

	// Attach the FPS mesh to the FPS camera.
	FPSMesh->SetupAttachment(AimCamera);

	// Disable some environmental shadows to preserve the illusion of having a single mesh.
	FPSMesh->bCastDynamicShadow = false;
	FPSMesh->CastShadow = false;

	//camera setup 
	CameraBoom->Activate();
	FollowCamera->Activate();
	AimCamera->Deactivate();

	// The owning player doesn't see the regular (third-person) body mesh.
	FPSMesh->SetHiddenInGame(true);
	GetMesh()->SetOwnerNoSee(false);

	// is set for 3rd person
	isFPSCamera = false;

	//don't show crosshair
	bShowCrosshair = false;

	//targets hit
	TargetsHit = 0;
	
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMyGameCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMyGameCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyGameCharacter::MoveRight);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AMyGameCharacter::FireWeapon);

	//handle cameras
	PlayerInputComponent->BindAction("1stView", IE_Pressed, this, &AMyGameCharacter::FirstViewCamera);
	PlayerInputComponent->BindAction("3rdView", IE_Pressed, this, &AMyGameCharacter::ThirdViewCamera);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMyGameCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMyGameCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AMyGameCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AMyGameCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AMyGameCharacter::OnResetVR);
}


void AMyGameCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AMyGameCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void AMyGameCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void AMyGameCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMyGameCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMyGameCharacter::MoveForward(float Value)
{
	
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		//FVector LocationRat = Controller->GetComponentLocation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMyGameCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

//----------------------------------------------------------------------------------------
void AMyGameCharacter::BeginPlay()
{
	Super::BeginPlay();

	/*AGameModeBase* gm = (AGameModeBase*)GetWorld()->GetAuthGameMode();
	gm*/

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	AMyGun *Spawner = GetWorld()->SpawnActor<AMyGun>(WeaponSpawn, SpawnParams);

	if (Spawner)
	{
		Spawner->AttachRootComponentTo(GetMesh(), "socketHold", EAttachLocation::SnapToTarget);
		CurrentWeapon = Spawner;
		if(!HasGun)
			CurrentWeapon->SetActorHiddenInGame(true);
	}

	//camera setup 
	CameraBoom->Activate();
	FollowCamera->Activate();
	AimCamera->Deactivate();

	FPSMesh->SetHiddenInGame(true);
	// The owning player doesn't see the regular (third-person) body mesh.
	GetMesh()->SetOwnerNoSee(false);
	//bool camera
	isFPSCamera = false;
	HasGun = false;
	bShowCrosshair = false;
	
}

void AMyGameCharacter::FireWeapon()
{
	if ((CurrentWeapon != NULL)&&(HasGun))
	{
		if (isFPSCamera)
		{
			CurrentWeapon->FireFirst(this, AimCamera);
		}
		else
		{
			CurrentWeapon->FireThird(this);
		}
		
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, "NO WEAPON IN HAND");
	}
}

void AMyGameCharacter::FirstViewCamera()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, "1ST CAMERA");

	CameraBoom->Deactivate();
	FollowCamera->Deactivate();
	AimCamera->Activate();
	isFPSCamera = true;

	if (!HasGun)
	{
		FPSMesh->SetHiddenInGame(true);
		bShowCrosshair = false;
	}
	else
	{
		FPSMesh->SetHiddenInGame(false);
		bShowCrosshair = true;
	}
	
	// The owning player doesn't see the regular (third-person) body mesh.
	GetMesh()->SetOwnerNoSee(true);
	CurrentWeapon->SetActorHiddenInGame(false);
}

void AMyGameCharacter::ThirdViewCamera()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, "3rd CAMERA");

	CameraBoom->Activate();
	FollowCamera->Activate();
	AimCamera->Deactivate();
	//bool camera
	isFPSCamera = false;

	FPSMesh->SetHiddenInGame(true);
	//hide crosshair
	bShowCrosshair = false;
	//enumShowCrosshair = 
	// The owning player doesn't see the regular (third-person) body mesh.
	GetMesh()->SetOwnerNoSee(false);
}


void AMyGameCharacter::OnOverlap(class UPrimitiveComponent* HitComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	if (OtherActor->IsA(AMyGun::StaticClass()))
	{
		//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, "Overlapped called");
		if (!HasGun)
		{
			if (CurrentWeapon) 
			{
				PlayGetGunSound(GetGunSound);

				CurrentWeapon->SetActorHiddenInGame(false);
				OtherActor->Destroy();

				if (isFPSCamera)
				{
					FPSMesh->SetHiddenInGame(false);
					bShowCrosshair = true;
				}

				CurrentWeapon->SetOwner(this);
				
				HasGun = true;

				GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, "GOT WEAPON");
			}
			else 
			{
				GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, "NO WEAPON");
			}
			
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, "ALREADY HAS WEAPON");
		}
	}

}

void AMyGameCharacter::PlayGetGunSound(USoundCue* sound)
{
	if (sound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, sound, GetActorLocation());
	}
}

