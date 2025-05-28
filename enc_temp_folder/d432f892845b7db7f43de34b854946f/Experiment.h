#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RobotBT/Actors/RoomPreparation/RobotCleaner.h"
#include "RobotBT/Actors/RoomPreparation/RobotOrganizer.h"
#include "RobotBT/Struct/ExperimentResult.h"
#include "RobotBT/Struct/TaskStruct.h"
#include "RobotBT/Struct/ValidationStruct.h"
#include "RobotBT/Struct/WorldRoomDataStruct.h"
#include "Experiment.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnExperimentFinished, FExperimentResult, ExperimentResult);

UCLASS(minimalapi)
class AExperiment : public AGameModeBase {
	GENERATED_BODY()

public:
	AExperiment();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	// the experiment
	UFUNCTION()
	virtual void ExecuteExperiment(FExperimentResult& NewExperiment);

	UFUNCTION()
	void ExperimentFinished(FExperimentResult CurrentExperiment);

	FOnExperimentFinished OnExperimentFinished;

};





