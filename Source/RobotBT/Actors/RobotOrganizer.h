#pragma once

#include "CoreMinimal.h"
#include "Robot.h"
#include "RobotOrganizer.generated.h"

class ARobotController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBatteryOver, bool, NewState);

UCLASS()
class ROBOTBT_API ARobotOrganizer : public ARobot {
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:	
	ARobotOrganizer();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
	bool IsOrganazing = false;

	UFUNCTION()
	void StartOrganizeTask(ARoom* Room);

	UFUNCTION()
	void TaskFinished(FString TaskMessage);


};
