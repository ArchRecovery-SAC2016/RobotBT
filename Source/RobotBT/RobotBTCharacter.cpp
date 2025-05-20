// Copyright Epic Games, Inc. All Rights Reserved.

#include "RobotBTCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/PlayerController.h"
#include "Materials/Material.h"
#include "Engine/World.h"

ARobotBTCharacter::ARobotBTCharacter() {


}

void ARobotBTCharacter::BeginPlay()
{
	Super::BeginPlay();

    APlayerController* PC = Cast<APlayerController>(GetController());
    if (PC) {
        PC->bShowMouseCursor = true;
        PC->bEnableClickEvents = true;
        PC->bEnableMouseOverEvents = true;

        // Permite movimentação E interação com UI simultaneamente
        PC->SetInputMode(FInputModeGameAndUI());
    }


}
