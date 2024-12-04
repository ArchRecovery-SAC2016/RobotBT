#pragma once

#include "CoreMinimal.h"
#include "../Robot.h"
#include "RobotCleaner.generated.h"


USTRUCT(BlueprintType)
struct FCleanerProperties {
	GENERATED_BODY()

	// Will be used to check if the robot is sanitized
	UPROPERTY(EditInstanceOnly, Category = "Robot")
	bool IsSanitized = false;
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

	virtual void StartNewTask(ESkillEnum SkillEnum, ARoom* Room) override;

	// Will generate randrom properties for the robot
	virtual void GenerateRandomProperties() override;

	bool IsSanitized() { return CleanerProperties.IsSanitized; }

	virtual bool TaskExecution() override;

};
