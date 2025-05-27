#include "Experiment.h"

#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "RobotBT/RobotBTPlayerController.h"
#include "UObject/ConstructorHelpers.h"
#include "RobotBT/Util/MyJsonReader.h"
#include "RobotBT/Util/UtilMethods.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFilemanager.h"
#include "RobotBT/Controllers/RoomPreparationBaseController.h"

AExperiment::AExperiment() {
	// use our custom PlayerController class
	PlayerControllerClass = ARobotBTPlayerController::StaticClass();

	// set default controller to our Blueprinted controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownPlayerController"));
	if (PlayerControllerBPClass.Class != NULL) {
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}

	PrimaryActorTick.bCanEverTick = true;
}

void AExperiment::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

void AExperiment::BeginPlay() {
	Super::BeginPlay();

	
}

void AExperiment::ExecuteExperiment(FExperimentResult& NewExperiment) {
	ARoomPreparationBaseController* Controller = GetWorld()->SpawnActor<ARoomPreparationBaseController>();
	Controller->ExecuteExperiment(NewExperiment);

	Controller->FOnPreparationFinish.AddDynamic(this, &AExperiment::ExperimentFinished);
}


void AExperiment::ExperimentFinished(FExperimentResult CurrentExperiment) {
	/*
	FValidationStruct Validation = GetValidationStruct();

	// Validate Experiment is a Assync method. So we need to wait it finished, and then we call Super:ExperimentFinished
	ValidateExperiment(Validation);
	*/

	OnExperimentFinished.Broadcast(CurrentExperiment);
}
