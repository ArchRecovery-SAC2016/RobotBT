#pragma once

#include "CoreMinimal.h"
#include "Robot.h"
#include "RobotCleaner.generated.h"

UCLASS()
class ROBOTBT_API ARobotCleaner : public ARobot {
	GENERATED_BODY()


public:

	ARobotCleaner();

	UPROPERTY()
	bool IsCleaning = false;

	virtual bool ProcessAction() override;

	virtual ADoorSensor* GetNextRoom() override;

};
