#pragma once
#include "CoreMinimal.h"
#include "GoalTracker.h"
#include "RobotBT/Enum/MessageColorEnum.h"
#include "RobotBT/Struct/TaskStruct.h"
#include "RobotBT/Struct/WorldRoomDataStruct.h"
#include "MyJsonReader.generated.h"

UCLASS()
class ROBOTBT_API  UMyJsonReader: public UBlueprintFunctionLibrary {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "MyUtils")
	static  TMap<FString, FTask> ReadTaskFromFile(FString Experiment, int32 ScenarioId);

	UFUNCTION(BlueprintCallable, Category = "MyUtils")
	static void WriteStringToFile(FString FilePath, FString String);

	static TMap<FString, FTask> ReadTasks(const TSharedPtr<FJsonObject>& TasksObject);

	static void ShowLogMessage(const FString& Message, EMessageColorEnum Type);

	UFUNCTION(BlueprintCallable, Category = "MyUtils")
	static TArray<FWorldRoomDataStruct> LoadWorldData(FString Experiment, int32 ScenarioId);

	static FString ReadStringFromFile(FString FilePath);

	// Load the goal mode from a json string
	UFUNCTION(BlueprintCallable, Category = "Goal Evaluator")
	FGoalModel LoadGoalModelFromJson(const FString& JsonString);

};
