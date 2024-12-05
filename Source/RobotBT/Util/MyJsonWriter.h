#pragma once
#include "CoreMinimal.h"
#include "RobotBT/Struct/ExperimentResult.h"
#include "MyJsonWriter.generated.h"

USTRUCT(BlueprintType)
struct FToJson {
	GENERATED_BODY()

	UPROPERTY()
	TArray<FExperimentResult> Result;

};

UCLASS()
class ROBOTBT_API  UMyJsonWriter: public UBlueprintFunctionLibrary {
	GENERATED_BODY()

public:
	UFUNCTION()
	static void AddToJsonFile(TArray<FExperimentResult> Results, FString ExperimentName, int32 ScenarioId);

private:
	inline static FString FilePath = "";

};
