#pragma once
#include "CoreMinimal.h"
#include "RobotBT/Enum/MessageColorEnum.h"
#include "UtilMethods.generated.h"

UCLASS()
class ROBOTBT_API  UUtilMethods: public UBlueprintFunctionLibrary {
	GENERATED_BODY()

public:
	static void ShowLogMessage(const FString& Message, EMessageColorEnum Type);
};
