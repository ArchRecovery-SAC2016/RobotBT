// Copyright Epic Games, Inc. All Rights Reserved.

#include "RobotBTCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/PlayerController.h"
#include "Materials/Material.h"
#include "Engine/World.h"

ARobotBTCharacter::ARobotBTCharacter() {
	Super::BeginPlay();

	// Mostra o ponteiro do mouse
	if (APlayerController* PC = Cast<APlayerController>(GetController())) {
		PC->bShowMouseCursor = true;
		PC->bEnableClickEvents = true;
		PC->bEnableMouseOverEvents = true;
		PC->DefaultMouseCursor = EMouseCursor::Default;
	}
}

void ARobotBTCharacter::BeginPlay()
{
	Super::BeginPlay();
}
