#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "RobotController.generated.h"

class ADoorSensor;

UCLASS()
class ARobotController : public AAIController
{
	GENERATED_BODY()

public:
	ARobotController();

	virtual void BeginPlay() override;

	// Save the rooms that are prepared
	TArray<ADoorSensor*> Is_Prepared;

	void GoToNextDoor();

};


