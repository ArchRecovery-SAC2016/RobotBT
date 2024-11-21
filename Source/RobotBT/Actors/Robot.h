#pragma once

#include "CoreMinimal.h"
#include "Room.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Character.h"
#include "RobotBT/Struct/RobotProperties.h"
#include "RobotBT/Enum/FailureReasonEnum.h"
#include "Robot.generated.h"


class ARobotController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTaskFailed, EFailureReasonEnum, ReasonEnum, FRobotProperties, RobotProperties);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTaskFinished, FRobotProperties, RobotProperties);


UCLASS()
class ROBOTBT_API ARobot : public ACharacter {
	GENERATED_BODY()


protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	ARobotController* RobotController = nullptr;

	ARobotController* GetRobotController();

public:	
	ARobot();

	virtual void Tick(float DeltaTime) override;

	// the widget that will be used to show the robot information
	UPROPERTY(EditDefaultsOnly, Category = "Robot")
	TSubclassOf<class URobotWidget> RobotWidgetWBP;

	UPROPERTY(EditDefaultsOnly, Category = "Robot")
	UWidgetComponent* WidgetComponent;

	UPROPERTY(EditAnywhere, Category = "Robot")
	FRobotProperties RobotProperties;

	UPROPERTY(EditAnywhere, Category = "Robot")
	bool ShowWidget = true;

	// indicate if the robot is moving
	UPROPERTY()
	bool IsMoving = false;

	UPROPERTY()
	FOnTaskFinished OnTaskFinished;

	UPROPERTY()
	FOnTaskFailed OnTaskFailed;

	UFUNCTION()
	virtual ARoom* GetRoom();

	UFUNCTION()
	virtual void SetRoom(ARoom* NewRoomInstance) { RoomInstace = NewRoomInstance; }

	// this is the world knowledge of this robot. It will be used to make decisions
	UFUNCTION()
	virtual USplineComponent* GetRoomPath();

	UFUNCTION()
	void UpdateRobotWidget();

	UFUNCTION()
	URobotWidget* GetRobotWidget();

	virtual void ExecuteTask(ESkillEnum SkillType, ARoom* Room);

protected:
	// indicate if the robot finished the action of move to a specific door
	UPROPERTY()
	bool IsAtRoomLocation = false;

	// indicate if the robot move all path
	UPROPERTY()
	bool IsFinishedMovingAlongPath = false;

	UFUNCTION()
	virtual bool MoveToRoomLocation(float DeltaTime);

	UFUNCTION()
	virtual bool MoveAlongPath(float DeltaTime);

	UFUNCTION()
	void UpdateCurrentActionText(FString NewAction);

	// Generate Random properties comuns to both robots
	UFUNCTION()
	virtual void GenerateRandomProperties();

private:
	UFUNCTION()
	void ConsumeBattery(float DeltaTime);

	// saves a instance of the current spline path
	UPROPERTY()
	ARoom* RoomInstace;

	// saves a instance of the Robot Widget
	UPROPERTY()
	URobotWidget* RobotWidget = nullptr;

	UPROPERTY()
	FText CurrentAction;

};
