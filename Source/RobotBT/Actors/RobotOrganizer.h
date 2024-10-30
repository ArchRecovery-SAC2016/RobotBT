#pragma once

#include "CoreMinimal.h"
#include "DoorSensor.h"
#include "FurniturePlace.h"
#include "Robot.h"
#include "RobotOrganizer.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFurnitureMoveEnded, bool, NewState);

UCLASS()
class ROBOTBT_API ARobotOrganizer : public ARobot {
	GENERATED_BODY()

public:
	ARobotOrganizer();

	
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY()
	bool IsOrganizing = false;

	virtual void ProcessAction() override;

	// Called by the Game Mode to save witch Furnitues must be moved. It will increment the FurnitureToMove array
	void AddMoveTask(AFurniturePlace* Furniture);

	// Called by the Game Mode to start the move furniture action.
	void StartMoveFurniture(ADoorSensor* Room);

	FOnFurnitureMoveEnded OnFurnitureMoveEnded;

	bool IsRobotMovingFurniture() { return IsMovingFurniture; }

private:
	UPROPERTY()
	TArray<AFurniturePlace*> FurnitureToMove;

	UPROPERTY()
	bool IsMovingFurniture = false;

	// Called by tick function. Will move the furniture to the correct location. When end will set IsMovingFurniture to false
	void MoveFurniture();


};
