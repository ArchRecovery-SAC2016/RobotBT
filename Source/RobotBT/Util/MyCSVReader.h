#pragma once
#include "CoreMinimal.h"
#include "RobotBT/Struct/ExperimentResult.h"
#include "MyCSVReader.generated.h"

UCLASS()
class ROBOTBT_API  UMyCSVReader: public UBlueprintFunctionLibrary {
	GENERATED_BODY()

public:
	UFUNCTION()
	static void CreateCSVFile(FString ExperimentName, int32 ScenarioId);

	UFUNCTION()
	static void AddToFile(FExperimentResult Result);

	inline static FString FilePath = "";
};
