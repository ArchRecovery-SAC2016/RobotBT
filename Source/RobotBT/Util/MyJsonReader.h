#pragma once
#include "CoreMinimal.h"
#include "RobotBT/Struct/TaskStruct.h"
#include "MyJsonReader.generated.h"

UCLASS()
class ROBOTBT_API  UMyJsonReader: public UBlueprintFunctionLibrary {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "MyUtils")
	static  TMap<FString, FTask> ReadJsonFile();

	UFUNCTION(BlueprintCallable, Category = "MyUtils")
	static FString ReadStringFromFile(FString FilePath);

	UFUNCTION(BlueprintCallable, Category = "MyUtils")
	static void WriteStringToFile(FString FilePath, FString String);

	static TMap<FString, FTask> ReadTasks(const TSharedPtr<FJsonObject>& TasksObject);

};
