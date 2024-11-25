#pragma once

#include "CoreMinimal.h"
#include "RoomPreparation.h"
#include "../Robot.h"
#include "RobotCleaner.generated.h"


USTRUCT(BlueprintType)
struct FCleanerProperties {
	GENERATED_BODY()

	// Will be used to check if the robot is sanitized
	UPROPERTY(EditInstanceOnly, Category = "Robot")
	bool IsSanitized = false;

	UPROPERTY()
	bool IsSanitizing = false;

	UPROPERTY()
	bool IsOpeningDoor = false;

	UPROPERTY()
	bool IsCleaning = false;

};

UCLASS()
class ROBOTBT_API ARobotCleaner : public ARobot{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:	
	ARobotCleaner();

	virtual void Tick(float DeltaTime) override;

	FCleanerProperties CleanerProperties;

	virtual void ExecuteTask(ESkillEnum SkillEnum, ARoom* Room) override;

	// Will generate randrom properties for the robot
	virtual void GenerateRandomProperties() override;

	bool IsSanitized() { return CleanerProperties.IsSanitized; }

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
