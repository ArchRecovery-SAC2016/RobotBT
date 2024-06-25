/** This is a list of all types that exits in the game*/
#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EMessageColorEnum: uint8 {
	NONE			UMETA(DisplayName = "NONE"),
	ERROR			UMETA(DisplayName = "ERROR"),
	WARNING			UMETA(DisplayName = "WARNING"),
	SUCCESS			UMETA(DisplayName = "SUCCESS"),
	INFO 			UMETA(DisplayName = "INFO")
    
};
