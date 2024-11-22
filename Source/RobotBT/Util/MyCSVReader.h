#pragma once
#include "CoreMinimal.h"
#include "RobotBT/Enum/FailureReasonEnum.h"
#include "RobotBT/Struct/RobotProperties.h"
#include "MyCSVReader.generated.h"

UCLASS()
class ROBOTBT_API  UMyCSVReader: public UBlueprintFunctionLibrary {
	GENERATED_BODY()

public:
	UFUNCTION()
	static void WriteCSVFile(FRobotProperties RobotProperties, EFailureReasonEnum FailureReason);

	UFUNCTION()
	static void CreateCSVFile(bool NewFile);
};
