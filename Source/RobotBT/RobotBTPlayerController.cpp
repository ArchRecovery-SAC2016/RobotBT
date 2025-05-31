#include "RobotBTPlayerController.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"

ARobotBTPlayerController::ARobotBTPlayerController() {


}

void ARobotBTPlayerController::BeginPlay() {
	// Call the base class  
	Super::BeginPlay();

    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;

    // Permite movimentação E interação com UI simultaneamente
    FInputModeGameAndUI InputMode;
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    InputMode.SetHideCursorDuringCapture(false);
    SetInputMode(InputMode);
}




