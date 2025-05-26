#pragma once

#include "CoreMinimal.h"
#include "ValidationStruct.generated.h"

USTRUCT(BlueprintType)
struct FRoomAssignment {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TArray<FString> assignedRobots;
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
	TArray<FString> robots_organizer;

	UPROPERTY(BlueprintReadWrite)
	TMap<FString, bool> room_clean_initial_state;

	UPROPERTY(BlueprintReadWrite)
	TMap<FString, bool> room_clean_final_state;

	UPROPERTY(BlueprintReadWrite)
	TMap<FString, bool> room_organize_initial_state;

	UPROPERTY(BlueprintReadWrite)
	TMap<FString, bool> room_organize_final_state;

	UPROPERTY(BlueprintReadWrite)
	TMap<FString, bool> door_open_inicial;

	UPROPERTY(BlueprintReadWrite)
	TMap<FString, bool> door_open_final;

	UPROPERTY(BlueprintReadWrite)
	TMap<FString, FRoomAssignment> cleaning_assignments;

	UPROPERTY(BlueprintReadWrite)
	TMap<FString, FRoomAssignment> sanitization_tasks;

	UPROPERTY(BlueprintReadWrite)
	int32 min_organizers = 0;

	UPROPERTY(BlueprintReadWrite)
	int32 max_organizers = 0;

	UPROPERTY(BlueprintReadWrite)
	int32 min_cleaners = 0;

	UPROPERTY(BlueprintReadWrite)
	int32 max_cleaners = 0;
};