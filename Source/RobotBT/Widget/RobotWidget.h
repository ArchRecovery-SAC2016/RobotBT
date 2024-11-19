#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RobotBT/Actors/Robot.h"
#include "RobotBT/Struct/RoomPreparationStruct.h"
#include "RobotWidget.generated.h"

/**
 * 
 */
UCLASS()
class ROBOTBT_API URobotWidget : public UUserWidget {
	GENERATED_BODY()

protected:
	// Doing setup in the C++ constructor is not as
	virtual void NativeConstruct() override;

private:
	// the actual battery level
	float Battery = 0;

	// the current action
	UPROPERTY()
	FText Action;

public:
	// set the battery level
	void SetBattery(float BatteryLevel);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetBattery();

	void SetAction(FText NewAction);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FText GetAction();
};
