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

	UPROPERTY(EditInstanceOnly, Category = "MyComponent")
	TArray<ADoorSensor*> DoorSensor;

	UFUNCTION()
	TArray<ADoorSensor*> GetRoomsToBePrepared();

	UFUNCTION(BlueprintCallable)
	virtual ADoorSensor* GetNextRoom();

	/* The child will know how to process the action */
	UFUNCTION()
	virtual bool ProcessAction();

	UFUNCTION(BlueprintCallable)
	ADoorSensor* GetNextRoomToBePrepared();


};
