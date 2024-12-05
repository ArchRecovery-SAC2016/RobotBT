#pragma once

#include "CoreMinimal.h"
#include "Room.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Character.h"
#include "RobotBT/Struct/RobotProperties.h"
#include "RobotBT/Struct/ExperimentResult.h"
#include "Robot.generated.h"


class ARobotController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTaskFinished, FTaskResult, TaskResult);

UCLASS()
class ROBOTBT_API ARobot : public ACharacter {
	GENERATED_BODY()


protected:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

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
	UPROPERTY(BlueprintReadWrite)
	bool IsMoving = false;

	UPROPERTY()
	FOnTaskFinished OnTaskFinished;

	UFUNCTION(BlueprintCallable)
	virtual ARoom* GetRoom();

	// some robots need to move to a specific location in the room
	UFUNCTION()
	virtual USplineComponent* GetRoomPath();

	UFUNCTION()
	virtual void SetRoom(ARoom* NewRoomInstance) { RoomInstace = NewRoomInstance; }

	UFUNCTION()
	void UpdateRobotWidget();

	UFUNCTION()
	URobotWidget* GetRobotWidget();

	// initiate the robot. Called when start a new experiment. Will set the properties and move to the editor position
	UFUNCTION()
	void Initiate(bool bGenerateRandomProperties);

	virtual void StartNewTask(ESkillEnum SkillType, ARoom* Room);

	// The result of task execution
	FTaskResult TaskResult;

	// saves the editor properties
	FRobotProperties EditorRobotProperties;

	/* Called by the controler, to tell the that the wall time is over */
	UFUNCTION()
	void TimeIsOver();
protected:
	// Callend when need to room entrance. Used by BTTAsk_MoveToRoomLocation. Return true when finished
	UFUNCTION(BlueprintCallable)
	bool MoveToRoomEntrance();

	// Called when need to move along a path. 
	UFUNCTION(BlueprintCallable)
	virtual bool MoveAlongPath();

	UFUNCTION()
	void UpdateCurrentActionText(FString NewAction);

	// Generate Random properties comuns to both robots
	UFUNCTION()
	virtual void GenerateRandomProperties();

	UFUNCTION()
	virtual  void TaskFailed(EFailureReasonEnum FailureReason);

	UFUNCTION(BlueprintCallable)
	virtual void TaskFinished();

	UFUNCTION(BlueprintCallable)
	virtual bool TaskExecution();

	UPROPERTY(BlueprintReadOnly)
	ESkillEnum TaskAllocated = ESkillEnum::NONE;

	UFUNCTION(BlueprintCallable)
	void GoIdle();


private:
	// indicate if the robot move all path
	UPROPERTY()
	bool IsFinishedMovingAlongPath = false;

	// indicate if the robot finished the action of move to a specific door
	UPROPERTY()
	bool IsAtRoomLocation = false;

	// saves a instance of the current spline path
	UPROPERTY()
	ARoom* RoomInstace = nullptr;

	// saves a instance of the Robot Widget
	UPROPERTY()
	URobotWidget* RobotWidget = nullptr;

	UPROPERTY()
	FText CurrentAction;

	UFUNCTION()
	void ConsumeBattery(float DeltaTime);

};
