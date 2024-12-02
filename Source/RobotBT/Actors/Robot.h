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

	// indicate if the robot finished the action of move to a specific door
	UPROPERTY(BlueprintReadWrite)
	bool IsAtRoomLocation = false;

	UPROPERTY(EditAnywhere, Category = "Robot")
	bool ShowWidget = true;

	// indicate if the robot is moving
	UPROPERTY()
	bool IsMoving = false;

	UPROPERTY()
	FOnTaskFinished OnTaskFinished;

	UFUNCTION(BlueprintCallable)
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

	// The result of task execution
	FTaskResult TaskResult;

	// saves the editor properties
	FRobotProperties EditorRobotProperties;

protected:

	// indicate if the robot move all path
	UPROPERTY(BlueprintReadOnly)
	bool IsFinishedMovingAlongPath = false;

	UFUNCTION(BlueprintPure)
	virtual const FVector GetRoomEntrance();

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
	UFUNCTION()
	void ConsumeBattery(float DeltaTime);

	// saves a instance of the current spline path
	UPROPERTY()
	ARoom* RoomInstace = nullptr;

	// saves a instance of the Robot Widget
	UPROPERTY()
	URobotWidget* RobotWidget = nullptr;

	UPROPERTY()
	FText CurrentAction;


};
