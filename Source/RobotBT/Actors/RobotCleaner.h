#pragma once

#include "CoreMinimal.h"
#include "Robot.h"
#include "Room.h"
#include "RobotCleaner.generated.h"

class ARobotController;


UCLASS()
class ROBOTBT_API ARobotCleaner : public ARobot{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:	
	ARobotCleaner();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
	bool IsSanitizing = false;

	UPROPERTY()
	bool IsOpeningDoor = false;

	UPROPERTY()
	bool IsCleaning = false;

	UFUNCTION()
	void StartSanitizationTask(ARoom* Room);

	UFUNCTION()
	void StartOpenDoorTask(ARoom* Room);

	UFUNCTION()
	void StartCleaninTask(ARoom* Room);


};
