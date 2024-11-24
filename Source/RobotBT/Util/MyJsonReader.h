#pragma once
#include "CoreMinimal.h"
#include "RobotBT/Enum/MessageColorEnum.h"
#include "RobotBT/Struct/TaskStruct.h"
#include "MyJsonReader.generated.h"

UCLASS()
class ROBOTBT_API  UMyJsonReader: public UBlueprintFunctionLibrary {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "MyUtils")
	static  TMap<FString, FTask> ReadTaskFromFile(FString Experiment, int32 ScenarioId);

	UFUNCTION(BlueprintCallable, Category = "MyUtils")
	static FString ReadStringFromFile(FString FilePath);

	UFUNCTION(BlueprintCallable, Category = "MyUtils")
	static void WriteStringToFile(FString FilePath, FString String);

	static TMap<FString, FTask> ReadTasks(const TSharedPtr<FJsonObject>& TasksObject);

	static void ShowLogMessage(const FString& Message, EMessageColorEnum Type);
};
