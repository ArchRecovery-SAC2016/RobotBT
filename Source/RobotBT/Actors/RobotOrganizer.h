#pragma once

#include "CoreMinimal.h"
#include "Robot.h"
#include "RobotOrganizer.generated.h"


UCLASS()
class ROBOTBT_API ARobotOrganizer : public ARobot {
	GENERATED_BODY()

public:
	ARobotOrganizer();

	UPROPERTY()
	bool IsOrganizing = false;

	virtual bool ProcessAction() override;
};
