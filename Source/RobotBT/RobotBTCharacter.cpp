// Copyright Epic Games, Inc. All Rights Reserved.

#include "RobotBTCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"

ARobotBTCharacter::ARobotBTCharacter() {
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = true;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bConstrainToPlane = false;
	GetCharacterMovement()->bSnapToPlaneAtStart = false;
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a top-down camera
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Tick enabled
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ARobotBTCharacter::MoveForward(float Value) {
	if (Controller && Value != 0.0f) {
		const FRotator Rotation = Controller->GetControlRotation();
		const FVector Direction = FRotationMatrix(Rotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ARobotBTCharacter::MoveRight(float Value) {
	if (Controller && Value != 0.0f) {
		const FRotator Rotation = Controller->GetControlRotation();
		const FVector Direction = FRotationMatrix(Rotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void ARobotBTCharacter::MoveUp(float Value) {
	if (Value != 0.0f) {
		AddMovementInput(FVector::UpVector, Value);
	}
}



void ARobotBTCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ARobotBTCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ARobotBTCharacter::MoveRight);
	PlayerInputComponent->BindAxis("MoveUp", this, &ARobotBTCharacter::MoveUp);
	PlayerInputComponent->BindAxis("Turn", this, &ARobotBTCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &ARobotBTCharacter::AddControllerPitchInput);
}

void ARobotBTCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}
