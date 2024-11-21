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

	UFUNCTION()
	void TaskFinished(FString TaskMessage);

	// override the GetRoomPath, because we don't want to pick the main path of the room
	virtual USplineComponent* GetRoomPath() override;

	// override the GetRoom, because we want to return the RoomPreparation instance
	virtual ARoom* GetRoom() override;

	// we need this to garantee that the NewRoomInstance is the type of ARoomPreparation
	virtual void SetRoom(ARoom* NewRoomInstance) override;

	virtual void ExecuteTask(ESkillEnum SkillEnum, ARoom* Room) override;

private:
	// we need this because the room organize use 2 paths 
	UPROPERTY()
	ARoomPreparation* RoomPreparation;

	UPROPERTY()
	bool IsOrganazing = false;

	UFUNCTION()
	void StartOrganizeTask(ARoomPreparation* Room);

};

