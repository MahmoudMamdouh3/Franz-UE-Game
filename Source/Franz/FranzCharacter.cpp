// Copyright Epic Games, Inc. All Rights Reserved.

#include "FranzCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Franz.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"

AFranzCharacter::AFranzCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	
	// --- CHANGED: The Claustrophobic Camera Setup ---
	CameraBoom->TargetArmLength = 150.0f; // Pulled in tight
	CameraBoom->SocketOffset = FVector(0.0f, 50.0f, 50.0f); // Shifted right over the shoulder
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
}

void AFranzCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFranzCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AFranzCharacter::Look);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFranzCharacter::Look);

		// --- NEW: Bind the Attack Action ---
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &AFranzCharacter::Attack);
	}
	else
	{
		UE_LOG(LogFranz, Error, TEXT("Failed to find Enhanced Input Component."));
	}
}

void AFranzCharacter::Attack()
{
	if (AttackMontage)
	{
		PlayAnimMontage(AttackMontage);
		
		// Set a timer to trigger the actual hitbox 0.4 seconds from now (when the fist extends)
		FTimerHandle AttackTimerHandle;
		GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &AFranzCharacter::Server_PerformMeleeHit, 0.4f, false);
	}
}

// --- The rest of the auto-generated movement math remains unchanged ---
void AFranzCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	DoMove(MovementVector.X, MovementVector.Y);
}

void AFranzCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AFranzCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void AFranzCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AFranzCharacter::DoJumpStart()
{
	Jump();
}

void AFranzCharacter::DoJumpEnd()
{
	StopJumping();
}

void AFranzCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// This tells the engine to load the inputs we set in the Blueprint
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

// Because it is a Server RPC, we MUST add "_Implementation" to the name here
void AFranzCharacter::Server_PerformMeleeHit_Implementation()
{
	FVector StartLoc = GetActorLocation();
	FVector ForwardVec = GetActorForwardVector();
	// Shoot the hitbox 100 units right in front of Franz's face
	FVector EndLoc = StartLoc + (ForwardVec * 100.0f); 

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this); // Make sure Franz doesn't punch himself

	// Draw a sphere trace (invisible boxing glove)
	bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult, 
		StartLoc, 
		EndLoc, 
		FQuat::Identity,
		ECC_Pawn, // We only want to punch things that are Pawns/Characters
		FCollisionShape::MakeSphere(40.0f), 
		QueryParams
	);

	// Draw a red debug sphere in the game so we can actually see the hitbox!
	DrawDebugSphere(GetWorld(), EndLoc, 40.0f, 12, FColor::Red, false, 2.0f);

	if (bHit && HitResult.GetActor())
	{
		// If we hit a Coworker, apply 25 damage to them!
		UGameplayStatics::ApplyDamage(HitResult.GetActor(), 25.0f, GetController(), this, UDamageType::StaticClass());
		
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("Punched: %s"), *HitResult.GetActor()->GetName()));
		}
	}
}