#pragma once

#include "CoreMinimal.h"
#include "RoomPreparation.h"
#include "../Robot.h"
#include "RobotOrganizer.generated.h"

class ARobotController;

UCLASS()
class ROBOTBT_API ARobotOrganizer : public ARobot {
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:	
	ARobotOrganizer();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditInstanceOnly, Category="Robot")
	int32 PathIndex = 1;

	// override the GetRoomPath, because we don't want to pick the main path of the room
	virtual USplineComponent* GetRoomPath() override;

	// override the GetRoom, because we want to return the RoomPreparation instance
	virtual ARoom* GetRoom() override;

	// we need this to garantee that the NewRoomInstance is the type of ARoomPreparation
	virtual void SetRoom(ARoom* NewRoomInstance) override;

	virtual void ExecuteTask(ESkillEnum SkillEnum, ARoom* Room) override;

	// Will generate randrom properties for the robot
	virtual void GenerateRandomProperties() override;

	virtual bool TaskExecution() override;

private:
	// we need this because the room organize use 2 paths 
	UPROPERTY()
	ARoomPreparation* RoomPreparation = nullptr;

	UPROPERTY()
	bool IsOrganazing = false;
};

