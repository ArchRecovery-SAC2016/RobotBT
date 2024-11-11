#pragma once

#include "CoreMinimal.h"
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

	FOnFurnitureMoveEnded OnFurnitureMoveEnded;

	bool IsRobotMovingFurniture() { return IsMovingFurniture; }

private:
	UPROPERTY()
	TArray<AFurniturePlace*> FurnitureToMove;

	UPROPERTY()
	bool IsMovingFurniture = false;



};
