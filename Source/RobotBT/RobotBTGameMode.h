// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Struct/RoomPreparationStruct.h"
#include "RobotBTGameMode.generated.h"

UCLASS(minimalapi)
class ARobotBTGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ARobotBTGameMode();
	virtual void BeginPlay() override;

	UFUNCTION()
	TArray<FRoomPreparationStruct> GetRooms() const { return Rooms;}

	UFUNCTION()
	void ChangeRoom(const FRoomPreparationStruct& RoomConfiguration);


private:
	/** The start widget. */
	// Saves all the rooms in the world
	TArray<FRoomPreparationStruct> Rooms;

	/** Load the start widget. */
	UFUNCTION()
	void LoadRoomsFromFile();
};



