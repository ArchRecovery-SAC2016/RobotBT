#include "RobotBTPlayerController.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"
#include "Blueprint/UserWidget.h"
#include "Widget/WorldKnowledgeWidget.h"

ARobotBTPlayerController::ARobotBTPlayerController() {


}

void ARobotBTPlayerController::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	WorldKnowledgeWidget = Cast<UWorldKnowledgeWidget>(CreateWidget(this, WorldKnowledgeWBP));

	if (WorldKnowledgeWidget == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("[UWidgetController::BeginPlay] WorldKnowledgeWidgetInst is null. You need to set WorldKnowledgeWBP at RobotController"));
		return;
	}

	WorldKnowledgeWidget->AddToViewport();

}




