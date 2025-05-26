#pragma once

#include "CoreMinimal.h"
#include "RobotProperties.h"
#include "ValidationStruct.generated.h"

USTRUCT(BlueprintType)
struct FRoomAssignment {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TArray<FString> AssignedRobots;
};

USTRUCT(BlueprintType)
struct FValidationStruct {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	int32 ExperimentId = 0;

	UPROPERTY(BlueprintReadWrite)
	TArray<FString> Rooms;

	UPROPERTY(BlueprintReadWrite)
	TArray<FString> Robots;

	UPROPERTY(BlueprintReadWrite)
	TArray<FString> RobotsOrganizer;

	UPROPERTY(BlueprintReadWrite)
	TMap<FString, bool> RoomCleanInitialState;

	UPROPERTY(BlueprintReadWrite)
	TMap<FString, bool> RoomCleanFinalState;

	UPROPERTY(BlueprintReadWrite)
	TMap<FString, bool> RoomOrganizeInitialState;

	UPROPERTY(BlueprintReadWrite)
	TMap<FString, bool> RoomOrganizeFinalState;

	UPROPERTY(BlueprintReadWrite)
	TMap<FString, bool> DoorOpenInicial;

	UPROPERTY(BlueprintReadWrite)
	TMap<FString, bool> DoorOpenFinal;

	UPROPERTY(BlueprintReadWrite)
	TMap<FString, FRoomAssignment> CleaningAssignments;

	UPROPERTY(BlueprintReadWrite)
	TMap<FString, FRoomAssignment> SanitizationTasks;

	UPROPERTY(BlueprintReadWrite)
	int32 MinOrganizers = 0;

	UPROPERTY(BlueprintReadWrite)
	int32 MaxOrganizers = 0;

	UPROPERTY(BlueprintReadWrite)
	int32 MinCleaners = 0;

	UPROPERTY(BlueprintReadWrite)
	int32 MaxCleaners = 0;


};