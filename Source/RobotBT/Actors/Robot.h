#pragma once

#include "CoreMinimal.h"
#include "DoorSensor.h"
#include "GameFramework/Character.h"
#include "Robot.generated.h"

UCLASS()
class ROBOTBT_API ARobot : public ACharacter {
	GENERATED_BODY()


protected:
	virtual void BeginPlay() override;

public:	
	ARobot();

	virtual void Tick(float DeltaTime) override;

	/* The child will know how to process the action */
	UFUNCTION()
	virtual bool ProcessAction();


};
