#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RobotBT/Struct/ExperimentResult.h"
#include "Experiment.generated.h"


UCLASS(minimalapi)
class AExperiment : public AGameModeBase {
	GENERATED_BODY()

public:
	AExperiment();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
	void ExecuteExperiment(FExperimentResult& NewExperiment);
};





