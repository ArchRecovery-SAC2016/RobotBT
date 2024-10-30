#pragma once

#include "CoreMinimal.h"
#include "Robot.h"
#include "RobotCleaner.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRoomCleaned, bool, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDoorOpened, bool, NewState);

UCLASS()
class ROBOTBT_API ARobotCleaner : public ARobot {
	GENERATED_BODY()


public:

	ARobotCleaner();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	FOnRoomCleaned OnRoomCleaned;

	FOnDoorOpened OnDoorOpened;

};
