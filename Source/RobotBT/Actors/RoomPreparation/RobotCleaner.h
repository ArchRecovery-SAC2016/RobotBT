#pragma once

#include "CoreMinimal.h"
#include "../Robot.h"
#include "RobotCleaner.generated.h"

class ARoomPreparation;
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

	virtual void ExecuteTask(FString SkillName, ARoom* Room) override;

private:
	UFUNCTION()
	void StartSanitizationTask(ARoomPreparation* Room);

	UFUNCTION()
	void StartOpenDoorTask(ARoomPreparation* Room);

	UFUNCTION()
	void StartCleaninTask(ARoomPreparation* Room);

	UFUNCTION()
	void TaskFinished(FString TaskMessage);


};
